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
#ifndef tet_metacube
#define tet_metacube

class Metacube {
public:
	enum		t_morph {MORPH_SPHERE, MORPH_PLANE, MORPH_CUBE, MORPH_TORUS, MORPH_DISTRIBUTION};
private:
	list<Particle>	pcube;
	list<Particle>	pplane;
	list<Particle>	psphere;
	list<Particle>	ptorus;
	list<Particle>	pdistribution;
	list<Particle>	p;
	vector<color3f>	colors;
	color3f		color;

	int		npoints;
	t_morph		morph;
	float		aspect;

	void BuildSphere(System &sys);
	void BuildPlane(System &sys);
	void BuildCube(System &sys);
	void BuildTorus(System &);
	void BuildDistribution(System &);

public:
	void Init(System &);

	void Morph(t_morph);

	void Update(System &);
	void Draw(Graphics &);
};

#endif
