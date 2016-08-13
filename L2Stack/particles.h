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
#ifndef tet_particles
#define tet_particles

struct Particle {
	int		timetolive;
	vector3f	p, v, f;

	Particle();
	Particle(const vector3f &);
};

class ParticleSystem {
	ParticleSystem();

public:
	static void Update(list<Particle> &);
	static void Update(list<Particle> &, float);
	static void Update(list<Particle> &, float, const vector3f &);
	static void Draw(const list<Particle> &);

	static void Add(list<Particle> &, int, const vector3f &);
	static void Agitate(RNG &, list<Particle> &, float);
	static void Align(list<Particle> &, const list<Particle> &, float);
	static void AlignRigid(list<Particle> &, const list<Particle> &, float);
};

#endif
