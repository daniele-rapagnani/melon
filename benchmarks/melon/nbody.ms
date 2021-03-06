let sun = {};
let jupiter = {};
let saturn = {};
let uranus = {};
let neptune = {};

let sqrt = math.sqrt;
let PI = 3.141592653589793;
let SOLAR_MASS = 4.0 * PI * PI;
let DAYS_PER_YEAR = 365.24;

sun.x = 0.0;
sun.y = 0.0;
sun.z = 0.0;
sun.vx = 0.0;
sun.vy = 0.0;
sun.vz = 0.0;
sun.mass = SOLAR_MASS;

jupiter.x = 4.84143144246472090e+00;
jupiter.y = -1.16032004402742839e+00;
jupiter.z = -1.03622044471123109e-01;
jupiter.vx = 1.66007664274403694e-03 * DAYS_PER_YEAR;
jupiter.vy = 7.69901118419740425e-03 * DAYS_PER_YEAR;
jupiter.vz = -6.90460016972063023e-05 * DAYS_PER_YEAR;
jupiter.mass = 9.54791938424326609e-04 * SOLAR_MASS;

saturn.x = 8.34336671824457987e+00;
saturn.y = 4.12479856412430479e+00;
saturn.z = -4.03523417114321381e-01;
saturn.vx = -2.76742510726862411e-03 * DAYS_PER_YEAR;
saturn.vy = 4.99852801234917238e-03 * DAYS_PER_YEAR;
saturn.vz = 2.30417297573763929e-05 * DAYS_PER_YEAR;
saturn.mass = 2.85885980666130812e-04 * SOLAR_MASS;

uranus.x = 1.28943695621391310e+01;
uranus.y = -1.51111514016986312e+01;
uranus.z = -2.23307578892655734e-01;
uranus.vx = 2.96460137564761618e-03 * DAYS_PER_YEAR;
uranus.vy = 2.37847173959480950e-03 * DAYS_PER_YEAR;
uranus.vz = -2.96589568540237556e-05 * DAYS_PER_YEAR;
uranus.mass = 4.36624404335156298e-05 * SOLAR_MASS;

neptune.x = 1.53796971148509165e+01;
neptune.y = -2.59193146099879641e+01;
neptune.z = 1.79258772950371181e-01;
neptune.vx = 2.68067772490389322e-03 * DAYS_PER_YEAR;
neptune.vy = 1.62824170038242295e-03 * DAYS_PER_YEAR;
neptune.vz = -9.51592254519715870e-05 * DAYS_PER_YEAR;
neptune.mass = 5.15138902046611451e-05 * SOLAR_MASS;

let bodies = [sun, jupiter, saturn, uranus, neptune];

func advance |bodies, nbody, dt| => {
	for (let i in 0..nbody) {
		let bi = bodies[i];
		
		let bix = bi.x;
		let biy = bi.y; 
		let biz = bi.z;
		let bimass = bi.mass;
		let bivx = bi.vx;
		let bivy = bi.vy; 
		let bivz = bi.vz;

		for (let j in i+1..nbody) {
			let bj = bodies[j];
			
			let dx = bix-bj.x;
			let dy = biy-bj.y;
			let dz = biz-bj.z;

			let dist2 = dx*dx + dy*dy + dz*dz;
			let mag = sqrt(dist2);
			mag = dt / (mag * dist2);

			let bm = bj.mass*mag;

			bivx = bivx - (dx * bm);
			bivy = bivy - (dy * bm);
			bivz = bivz - (dz * bm);
			bm = bimass*mag;
			bj.vx = bj.vx + (dx * bm);
			bj.vy = bj.vy + (dy * bm);
			bj.vz = bj.vz + (dz * bm);
		}

		bi.vx = bivx;
		bi.vy = bivy;
		bi.vz = bivz;
		bi.x = bix + dt * bivx;
		bi.y = biy + dt * bivy;
		bi.z = biz + dt * bivz;
	}
}

func energy |bodies, nbody| => {
	let e = 0.0;

	for (let i in 0..nbody) {
		let bi = bodies[i];
		let vx = bi.vx;
		let vy = bi.vy;
		let vz = bi.vz;
		let bim = bi.mass;

		e = e + (0.5 * bim * (vx*vx + vy*vy + vz*vz));

		for (let j in i+1..nbody) {
			let bj = bodies[j];
			
			let dx = bi.x-bj.x;
			let dy = bi.y-bj.y;
			let dz = bi.z-bj.z;

			let distance = sqrt(dx*dx + dy*dy + dz*dz);
			e = e - ((bim * bj.mass) / distance);
		}
	}

	return e;
}

func offsetMomentum |b, nbody| => {
	let px = 0.0;
	let py = 0.0;
	let pz = 0.0;

	for (let i in 0..nbody) {
		let bi = b[i];
		let bim = bi.mass;
		px = px + (bi.vx * bim);
		py = py + (bi.vy * bim);
		pz = pz + (bi.vz * bim);
	}

	b[0].vx = -px / SOLAR_MASS;
	b[0].vy = -py / SOLAR_MASS;
	b[0].vz = -pz / SOLAR_MASS;
}

let N = integer.fromString(array.lookup(cli.getArgs(), 0) ?? "500000");

let nbody = #bodies;

offsetMomentum(bodies, nbody);
io.print(energy(bodies, nbody));

for (let i in 0..N) {
	advance(bodies, nbody, 0.01);
}

io.print(energy(bodies, nbody));