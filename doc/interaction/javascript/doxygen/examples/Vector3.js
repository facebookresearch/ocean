/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * Author: Jan Herling, janherling@meta.com
 * This JavaScript file test the ocean scripting support for the Vector3 object.
 */

/**
 * Initialization function which is called once to initialize this script.
 */
function onOceanInitialize()
{
	ocean.info("JavaScript Vector3 test: ");

	ocean.info(" ");
	var vector0 = new Vector3(1, 2, 3);
	var vector1 = new Vector3(10, 11.5, 12);
	ocean.info("vector0 (x, y, z): (", vector0.string(), ")");
	ocean.info("vector1 (x, y, z): (", vector1.string(), ")");

	ocean.info(" ");
	ocean.info("Length test:");
	ocean.info("vector0.length(): ", vector0.length());
	ocean.info("vector1.length(): ", vector1.length());

	ocean.info(" ");
	ocean.info("Add test:");
	var vector = vector0.add(vector1);
	ocean.info("vector = vector0.add(vector1): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Subtract test:");
	var vector = vector0.subtract(vector1);
	ocean.info("vector = vector0.subtract(vector1): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Multiply test:");
	var vector = vector0.multiply(4);
	ocean.info("vector = vector0.multiply(4): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Dot product test:");
	var scalar = vector0.multiply(vector1);
	ocean.info("scalar = vector0.multiply(vector1): ", scalar);

	ocean.info(" ");
	ocean.info("Cross product test:");
	vector = vector0.cross(vector1);
	ocean.info("vector = vector0.cross(vector1): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Angle test (in rad):");
	scalar = vector0.angle(vector1);
	ocean.info("scalar = vector0.angle(vector1): ", scalar);

	ocean.info(" ");
	ocean.info("Normalized test");
	vector = vector0.normalized();
	ocean.info("vector = vector0.normalized(): (", vector.string(), ")");
	vector = vector1.normalized();
	ocean.info("vector = vector1.normalized(): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Normalize test");
	vector0.normalize();
	vector1.normalize();
	ocean.info("vector0.normalize(): (", vector0.string(), ")");
	ocean.info("vector1.normalize(): (", vector1.string(), ")");

	ocean.info(" ");
	ocean.info("Inverted test");
	vector = vector0.inverted();
	ocean.info("vector = vector0.inverted(): (", vector.string(), ")");
	vector = vector1.inverted();
	ocean.info("vector = vector1.inverted(): (", vector.string(), ")");

	ocean.info(" ");
	ocean.info("Invert test");
	vector0.invert();
	vector1.invert();
	ocean.info("vector0.invert(): (", vector0.string(), ")");
	ocean.info("vector1.invert(): (", vector1.string(), ")");
}
