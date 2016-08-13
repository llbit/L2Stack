/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
	license.txt for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tet_incs.h"

void Metacube::Init(System &sys)
{
	morph = MORPH_DISTRIBUTION;
	npoints = 1000;
	aspect = (float)sys.GetScript().ParseInt("profile.width") / (float)sys.GetScript().ParseInt("profile.height");

	BuildPlane(sys);
	BuildCube(sys);
	BuildSphere(sys);
	BuildTorus(sys);
	BuildDistribution(sys);

	p = pdistribution;
	for (int i = 0; i < TILE_COUNT; ++i) {
		colors.push_back(GetTileColor((tile_t)i));
	}
}

void Metacube::BuildSphere(System &sys)
{
	//int		sections = (int)ceil(pow((float)npoints, 0.5f));
	float		phi, theta, pi = acos(-1.f);
	vector3f	v;
	/*for (int i = 0; i < sections; ++i) {
		phi = (i+1) * (pi/(sections+1)) - pi/2;
		for (int j = 0; j < sections; ++j) {
			theta = j * (2*pi/sections);
			v = vector3f(cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi));
			psphere.push_back(Particle(v));
		}
	}*/
	for (int i = 0; i < npoints; ++i) {
		phi = sys.GetRNG().GetFloat() * pi - pi/2;
		theta = 2*pi * sys.GetRNG().GetFloat();
		v = vector3f(cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi));
		psphere.push_back(Particle(v));
	}
}

void Metacube::BuildTorus(System &sys)
{
	//int		sections = (int)ceil(pow((float)npoints, 0.5f));
	float		phi, theta, ratio = 0.3f, pi = acos(-1.f);
	vector3f	angle, v;
	/*for (int i = 0; i < sections; ++i) {
		phi = 2.f * pi * (float)(i+1)/(float)(sections);
		angle = vector3f(cos(phi), 0.f, sin(phi));
		for (int j = 0; j < sections; ++j) {
			theta = 2.f * pi * (float)(j+1)/(float)(sections);
			v = vector3f(0.f, sin(theta), 0.f) * ratio;
			v += angle * ((1-ratio) + cos(theta) * ratio);
			ptorus.push_back(Particle(v));
		}
	}*/
	for (int i = 0; i < npoints; ++i) {
		phi = 2.f * pi * sys.GetRNG().GetFloat();
		angle = vector3f(cos(phi), 0.f, sin(phi));
		theta = 2.f * pi * sys.GetRNG().GetFloat();
		v = vector3f(0.f, sin(theta), 0.f) * ratio;
		v += angle * ((1-ratio) + cos(theta) * ratio);
		ptorus.push_back(Particle(v));
	}
}

void Metacube::BuildPlane(System &sys)
{
	//int		sections = (int)ceil(pow((float)npoints, 0.5f));
	float		x, y;
	vector3f	v;
	/*for (int i = 0; i < sections; ++i) {
		x = (float)i / (float)sections;
		x = (x*2.f)-1.f;
		for (int j = 0; j < sections; ++j) {
			y = (float)j / (float)sections;
			y = (y*2.f)-1.f;
			v = vector3f(x,y,0.f) * 0.7068f;
			pplane.push_back(Particle(v));
		}
	}*/
	for (int i = 0; i < npoints; ++i) {
		x = sys.GetRNG().GetFloat();
		x = (x*2.f)-1.f;
		y = sys.GetRNG().GetFloat();
		y = (y*2.f)-1.f;
		v = vector3f(x,y,0.f) * 0.7068f;
		pplane.push_back(Particle(v));
	}
}

void Metacube::BuildCube(System &sys)
{
	int		sections = (int)ceil(pow((float)npoints, 0.3333f));
	float		x, y, z;
	vector3f	v;
	for (int i = 0; i < sections; ++i) {
		x = (float)i / (float)sections;
		x = (x*2.f)-1.f;
		for (int j = 0; j < sections; ++j) {
			y = (float)j / (float)sections;
			y = (y*2.f)-1.f;
			for (int k = 0; k < sections; ++k) {
				z = (float)k / (float)sections;
				z = (z*2.f)-1.f;
				v = vector3f(x,y,z) * 0.7068f;
				pcube.push_back(Particle(v));
			}
		}
	}
}

void Metacube::BuildDistribution(System &sys)
{
	float		x, y, z;
	vector3f	v;
	for (int i = 0; i < npoints; ++i) {
		x = sys.GetRNG().GetFloat();
		x = (x*2.f)-1.f;
		y = sys.GetRNG().GetFloat();
		y = (y*2.f)-1.f;
		z = sys.GetRNG().GetFloat();
		z = (z*2.f)-1.f;
		v = vector3f(x,y,z) * 0.7068f;
		pdistribution.push_back(Particle(v));
	}
}

void Metacube::Morph(t_morph n)
{
	morph = n;
}

void Metacube::Update(System & sys)
{
	static int	colorpicker;
	static int	time = 0;
	time = (time+1)%80;

	if (time == 0) {
		colorpicker = (colorpicker+1)%TILE_COUNT;
	}
	color = colors[colorpicker] * (1.f - time/80.f) +
		colors[(colorpicker+1)%TILE_COUNT] * (time/80.f);

	switch (morph) {
	case MORPH_PLANE:
		ParticleSystem::AlignRigid(p, pplane, 5.f);
		break;
	case MORPH_CUBE:
		ParticleSystem::AlignRigid(p, pcube, 5.f);
		break;
	case MORPH_SPHERE:
		ParticleSystem::AlignRigid(p, psphere, 5.f);
		break;
	case MORPH_TORUS:
		ParticleSystem::AlignRigid(p, ptorus, 5.f);
		break;
	case MORPH_DISTRIBUTION:
		ParticleSystem::AlignRigid(p, pdistribution, 5.f);
		break;
	}
	ParticleSystem::Update(p);
}

void Metacube::Draw(Graphics &)
{
	static	int time = 0;
	time = (time+1)%360;
	GLfloat fogColor[4]= {0.1f, 0.1f, 0.1f, 1.0f};

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(-aspect*0.6f, aspect*0.6f, -0.6f, 0.6f, 10, 30);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 19.f);
	glFogf(GL_FOG_END, 21.f);
	glEnable(GL_FOG);

	float	h = 0.01f, w = 0.01f;

	glPushMatrix();
	glTranslatef(0, 0, -20);
	glRotatef((float)time, 0.7f, 5.f, 3.f);
	list<Particle>::const_iterator	it = p.begin();
	glPointSize(3.f);
	glBegin(GL_POINTS);
	glColor3f(color.r, color.g, color.b);
	for (; it != p.end(); ++it) {
		glVertex3f(it->p.x, it->p.y, it->p.z);
	}
	glColor3f(1, 1, 1);
	glEnd();
	glPopMatrix();

	glDisable(GL_FOG);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
