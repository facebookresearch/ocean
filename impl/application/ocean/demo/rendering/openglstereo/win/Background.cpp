/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/openglstereo/win/Background.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/media/ConfigMedium.h"

using namespace Ocean;
using namespace Ocean::Media;

Background::~Background()
{
	release();
}

void Background::update()
{
	const FrameType updateFrameType = texture_.update();
	bool newFrameType = frameType_ != updateFrameType;

	if (updateFrameType.width() == 0 || updateFrameType.height() == 0)
		return;

	if (displayList_ == 0)
	{
		displayList_ = glGenLists(1);
		ocean_assert(glGetError() == GL_NO_ERROR);
		newFrameType = true;
	}

	if (newFrameType)
	{
		frameType_ = updateFrameType;

		PinholeCamera camera = IO::CameraCalibrationManager::get().camera(mediumUrl_, frameType_.width(), frameType_.height());

		const Scalar backgroundFarClippingPlane = 100;
		const unsigned int backgroundNumberHorizontalStrips = 100;
		const unsigned int backgroundNumberVerticalStrips = 100;

		// distance is 95% of the far distance
		const Scalar distance = backgroundFarClippingPlane * Scalar(0.95);

		const Scalar left = -tan(camera.fovXLeft()) * distance;
		const Scalar right = tan(camera.fovXRight()) * distance;
		const Scalar top = tan(camera.fovYTop()) * distance;
		const Scalar bottom = -tan(camera.fovYBottom()) * distance;

		const Scalar horizontalStep = (right - left) / backgroundNumberHorizontalStrips;
		const Scalar verticalStep = (top - bottom) / backgroundNumberVerticalStrips;

		glNewList(displayList_, GL_COMPILE);
		ocean_assert(glGetError() == GL_NO_ERROR);

		for (unsigned int y = 0; y < backgroundNumberVerticalStrips; y++)
		{
			glBegin(GL_QUAD_STRIP);

			for (unsigned int x = 0; x <= backgroundNumberHorizontalStrips; x++)
			{
				/*glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-2.0f, 2.0f, 0.0f);

					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-2.0f, -2.0f, 0.0f);

					glTexCoord2f(1.0f, 0.0f);
					glVertex3f(2.0f, -2.0f, 0.0f);

					glTexCoord2f(1.0f, 1.0f);
					glVertex3f(2.0f, 2.0f, 0.0f);
				glEnd();
				ocean_assert(glGetError() == GL_NO_ERROR);*/

				const Vector2 coordinateTop(Scalar(x) / Scalar(backgroundNumberHorizontalStrips), Scalar(y) / Scalar(backgroundNumberVerticalStrips));
				const Vector2 coordinateBottom(Scalar(x) / Scalar(backgroundNumberHorizontalStrips), Scalar(y + 1) / Scalar(backgroundNumberVerticalStrips));

				glTexCoord2f(float(coordinateTop.x()), float(1 - coordinateTop.y()));
				glVertex3f(float(left + x * horizontalStep), float(top - y * verticalStep), float(-distance));

				glTexCoord2f(float(coordinateBottom.x()), float(1 - coordinateBottom.y()));
				glVertex3f(float(left + x * horizontalStep), float(top - (y + 1) * verticalStep), float(-distance));
			}

			glEnd();
			ocean_assert(glGetError() == GL_NO_ERROR);
		}

		glEndList();
		ocean_assert(glGetError() == GL_NO_ERROR);
	}
}

void Background::draw()
{
	if (displayList_ == 0)
	{
		return;
	}

	texture_.bind();
	glCallList(displayList_);
}

void Background::release()
{
	texture_.release();
}

void Background::setMedium(const FrameMediumRef& medium)
{
	texture_.setMedium(medium);

	if (medium)
	{
		mediumUrl_ = medium->url();
	}
	else
	{
		mediumUrl_.clear();
	}

	if (displayList_ != 0)
	{
		glDeleteLists(displayList_, 1);
		ocean_assert(glGetError() == GL_NO_ERROR);

		displayList_ = 0;
	}
}

void Background::configurateMedium(const unsigned int index)
{
	ConfigMediumRef configMedium(texture_.medium());

	if (configMedium)
	{
		ConfigMedium::ConfigNames configs = configMedium->configs();

		if (index < configs.size())
		{
			configMedium->configuration(configs[index], (long long)(nullptr));
		}
	}
}
