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

Particle::Particle()
{
	timetolive = 120;
	p = v = f = vector3f(0.f, 0.f, 0.f);
}

Particle::Particle(const vector3f & position)
{
	timetolive = 120;
	p = position;
	v = f = vector3f(0.f, 0.f, 0.f);
}

ParticleSystem::ParticleSystem()
{
}

void ParticleSystem::Update(list<Particle> & particles)
{
	list<Particle>::iterator	it = particles.begin();
	float				d = 1.f / 60.f;
	while (it != particles.end()) {
		if (--(it->timetolive) <= 0) {
			it = particles.erase(it);
			continue;
		}

		it->v += it->f * d;
		it->p += it->v * d;
		it->f = vector3f(0.f, 0.f, 0.f);

		++it;
	}
}

void ParticleSystem::Update(list<Particle> & particles, float drag)
{
	list<Particle>::iterator	it = particles.begin();
	float				d = 1.f / 60.f;
	while (it != particles.end()) {
		if (--(it->timetolive) <= 0) {
			it = particles.erase(it);
			continue;
		}

		it->v += it->f * d;
		it->v -= it->v * drag * d;
		it->p += it->v * d;
		it->f = vector3f(0.f, 0.f, 0.f);

		++it;
	}
}

void ParticleSystem::Update(list<Particle> & particles, float drag, const vector3f & g)
{
	list<Particle>::iterator	it = particles.begin();
	float				d = 1.f / 60.f;
	while (it != particles.end()) {
		if (--(it->timetolive) <= 0) {
			it = particles.erase(it);
			continue;
		}

		it->v += g * d;
		it->v += it->f * d;
		it->v -= it->v * drag * d;
		it->p += it->v * d;
		it->f = vector3f(0.f, 0.f, 0.f);

		++it;
	}
}

void ParticleSystem::Draw(const list<Particle> &particles)
{
	list<Particle>::const_iterator	it = particles.begin();
	for (; it != particles.end(); ++it) {
		glPushMatrix();
		glTranslatef(it->p.x, it->p.y, it->p.z);
		glBegin(GL_QUADS);
		glColor3f(1.f, 1.f, 1.f);
		glVertex3f(-1.f, -1.f, 0.f);
		glVertex3f(1.f, -1.f, 0.f);
		glVertex3f(1.f, 1.f, 0.f);
		glVertex3f(-1.f, 1.f, 0.f);
		glEnd();
		glPopMatrix();
	}
}

void ParticleSystem::Add(list<Particle> & p, int n, const vector3f & pos)
{
	for (int i = 0; i < n; ++i) {
		p.push_back(Particle(pos));
	}
}

void ParticleSystem::Agitate(RNG &rng, list<Particle> & particles, float f)
{
	list<Particle>::iterator	it = particles.begin();
	for (; it != particles.end(); ++it) {
		float fx = f * 2 * (rng.GetFloat() - 0.5f);
		float fy = f * 2 * (rng.GetFloat() - 0.5f);
		float fz = f * 2 * (rng.GetFloat() - 0.5f);
		it->p += vector3f(fx, fy, fz);
	}
}

void ParticleSystem::Align(list<Particle> & a, const list<Particle> & b, float f)
{
	list<Particle>::iterator	ita = a.begin();
	list<Particle>::const_iterator	itb = b.begin();
	for (; ita != a.end() && itb != b.end(); ++ita, ++itb) {
		ita->f += (itb->p - ita->p) * f;
		ita->timetolive += 1;
	}
}

void ParticleSystem::AlignRigid(list<Particle> & a, const list<Particle> & b, float f)
{
	list<Particle>::iterator	ita = a.begin();
	list<Particle>::const_iterator	itb = b.begin();
	float				d = f / 60.f;
	for (; ita != a.end() && itb != b.end(); ++ita, ++itb) {
		ita->p += (itb->p - ita->p) * d;
		ita->timetolive += 1;
	}
}
