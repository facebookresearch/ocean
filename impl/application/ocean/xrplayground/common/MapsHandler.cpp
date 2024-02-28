// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/MapsHandler.h"

#include "ocean/io/maps/rendering/Mesher.h"

#ifndef OCEAN_PLATFORM_BUILD_MOBILE
	#include "ocean/network/HTTPSClient.h"
#endif

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace XRPlayground
{

Rendering::TransformRef MapsHandler::createTile(const Rendering::Engine& engine, const IO::Maps::Basemap::Tile& tile, const Scalar targetSize, const bool volumetric)
{
	ocean_assert(tile.isValid());
	ocean_assert(targetSize > Numeric::eps());

	if (!tile.isValid() || targetSize <= Numeric::eps())
	{
		return Rendering::TransformRef();
	}

	const double tileMetricExtent = tile.metricExtent();

	const IO::Maps::Basemap::SharedObjects& objects = tile.objects();

	Rendering::TransformRef transform = engine.factory().createTransform();

	if (Rendering::TrianglesRef triangles = createBuildings(engine, objects, tileMetricExtent, targetSize, volumetric))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(0.5f, 0.5f, 0.5f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	if (Rendering::TrianglesRef triangles = createRoads(engine, objects, tileMetricExtent, targetSize))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(1.0f, 1.0f, 1.0f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	if (Rendering::TrianglesRef triangles = createTransits(engine, objects, tileMetricExtent, targetSize))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(1.0f, 1.0f, 1.0f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	if (Rendering::TrianglesRef triangles = createWaters(engine, objects, tileMetricExtent, targetSize))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(0.0f, 0.0f, 1.0f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	if (Rendering::TrianglesRef triangles = createLandUses(engine, objects, tileMetricExtent, targetSize))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(0.4f, 1.0f, 0.4f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	if (Rendering::TrianglesRef triangles = createLandCovers(engine, objects, tileMetricExtent, targetSize))
	{
		const Rendering::AttributeSetRef attributeSet = engine.factory().createAttributeSet();

		const Rendering::MaterialRef material = engine.factory().createMaterial();
		material->setDiffuseColor(RGBAColor(0.7f, 0.7f, 0.7f));
		attributeSet->addAttribute(material);

		const Rendering::GeometryRef geometry = engine.factory().createGeometry();
		geometry->addRenderable(triangles, attributeSet);

		transform->addChild(geometry);
	}

	return transform;
}

Rendering::TrianglesRef MapsHandler::createBuildings(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize, const bool volumetric)
{
	const Scalar groundPlaneOffset = Scalar(0.01); // raising buildings 1cm above ground level

	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createBuildings(objects, titleMetricExtent, targetSize, volumetric, groundPlaneOffset);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createBuilding(const Rendering::Engine& engine, const IO::Maps::Basemap::Building& building, const CV::PixelPositionI& origin, const Scalar pixelPositionNormalization, const Scalar metricNormalization, const bool volumetric)
{
	ocean_assert(origin.isValid());
	ocean_assert(pixelPositionNormalization > Numeric::eps());
	ocean_assert(metricNormalization > Numeric::eps());

	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createBuilding(building, origin, pixelPositionNormalization, metricNormalization, volumetric);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));

	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createRoads(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize)
{
	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createRoads(objects, titleMetricExtent, targetSize);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createTransits(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize)
{
	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createTransits(objects, titleMetricExtent, targetSize);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createWaters(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize)
{
	const Scalar groundPlaneOffset = Scalar(-0.01); // raising waters 1cm below ground level

	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createWaters(objects, titleMetricExtent, targetSize, groundPlaneOffset);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createLandUses(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize)
{
	const Scalar groundPlaneOffset = Scalar(-0.03); // raising land uses 3cm below ground level

	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createLandUses(objects, titleMetricExtent, targetSize, groundPlaneOffset);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

Rendering::TrianglesRef MapsHandler::createLandCovers(const Rendering::Engine& engine, const IO::Maps::Basemap::SharedObjects& objects, const double titleMetricExtent, const Scalar targetSize)
{
	const Scalar groundPlaneOffset = Scalar(-0.02); // raising land covers 2cm below ground level

	const IO::Maps::Rendering::Mesher::Mesh mesh = IO::Maps::Rendering::Mesher::createLandCovers(objects, titleMetricExtent, targetSize, groundPlaneOffset);

	if (!mesh.isValid())
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	vertexSet->set(mesh.vertices(), mesh.perVertexNormals(), Vectors2(), RGBAColors());

	Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	triangles->setFaces(Rendering::TriangleFace::indices2triangleFaces(mesh.triangleFaces().data(), mesh.triangleFaces().size()));
	triangles->setVertexSet(vertexSet);

	return triangles;
}

std::future<Network::Tigon::TigonRequest::TigonResponse> MapsHandler::downloadTile(const unsigned int level, const double latitude, const double longitude)
{
	ocean_assert(level >= 1u && level <= 22u);
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180 && longitude <= 180.0);

	const IO::Maps::Basemap::TileIndexPair tileIndexPair = IO::Maps::Basemap::Tile::calculateTile(level, latitude, longitude);
	ocean_assert(tileIndexPair.isValid());

	return downloadTile(level, tileIndexPair.latitudeIndex(), tileIndexPair.longitudeIndex());
}

std::future<Network::Tigon::TigonRequest::TigonResponse> MapsHandler::downloadTile(const unsigned int level, const unsigned int latitudeIndex, const unsigned int longitudeIndex)
{
	ocean_assert(level >= 1u && level <= 22u);

	const unsigned int numberTiles = IO::Maps::Basemap::Tile::numberTiles(level);

	if (latitudeIndex >= numberTiles || longitudeIndex >= numberTiles)
	{
		ocean_assert(false && "Invalid input!");
		return std::future<Network::Tigon::TigonRequest::TigonResponse>();
	}

	std::shared_ptr<std::promise<Network::Tigon::TigonRequest::TigonResponse>> stylePromise = std::make_shared<std::promise<Network::Tigon::TigonRequest::TigonResponse>>();
	Network::Tigon::TigonClient::ReponseCallbackFunction callback = [=](const Network::Tigon::TigonRequest::TigonResponse& response)
	{
		if (response.succeeded())
		{
			const std::string& styleData = response.response();
			std::string url;
			if (IO::Maps::Basemap::extractTileUrlTemplate(styleData.c_str(), styleData.length(), url))
			{
				const IO::Maps::Basemap::TileIndexPair tileIndexPair(latitudeIndex, longitudeIndex);
				if (!IO::Maps::Basemap::constructTileUrl(url, level, tileIndexPair, url) || url.empty())
				{
					Log::warning() << "Failed to construct tile url";
				}
			}
			else
			{
				Log::warning() << "Failed to get tile url template";
			}

			stylePromise->set_value(Network::Tigon::TigonClient::get().httpRequest(url).get());
		}
		else
		{
			Log::error() << "Failed to download map style";
			stylePromise->set_value(response);
		}
	};

	const std::string styleUrl = IO::Maps::Basemap::styleUrl();

	if (!Network::Tigon::TigonClient::get().httpRequest(styleUrl, std::move(callback)))
	{
		Log::error() << "Failed to submit map style request";
		return std::future<Network::Tigon::TigonRequest::TigonResponse>();
	}

	return stylePromise->get_future();
}

IO::Maps::Basemap::SharedTile MapsHandler::downloadTileDirect(const unsigned int level, const double latitude, const double longitude)
{
	ocean_assert(level >= 1u && level <= 22u);
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180 && longitude <= 180.0);

	if (level < 1u || level > 22u || latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0)
	{
		return nullptr;
	}

	const IO::Maps::Basemap::TileIndexPair tileIndexPair = IO::Maps::Basemap::Tile::calculateTile(level, latitude, longitude);
	ocean_assert(tileIndexPair.isValid());

#ifndef OCEAN_PLATFORM_BUILD_MOBILE

	const std::string styleUrl = IO::Maps::Basemap::styleUrl();
	std::vector<uint8_t> styleBuffer;
	if (!Network::HTTPSClient::httpsGetRequest(styleUrl, styleBuffer) || styleBuffer.empty())
	{
		Log::warning() << "Failed to download map style";
	}

	std::string url;
	if(!IO::Maps::Basemap::extractTileUrlTemplate(reinterpret_cast<const char*>(styleBuffer.data()), styleBuffer.size(), url))
	{
		Log::warning() << "Failed to get tile url template";
	}

	if (!IO::Maps::Basemap::constructTileUrl(url, level, tileIndexPair, url))
	{
		Log::warning() << "Failed to construct tile url";
	}

	std::vector<uint8_t> buffer;
	if (!Network::HTTPSClient::httpsGetRequest(url, buffer) || buffer.empty())
	{
		Log::warning() << "Failed to download map";
		return nullptr;
	}

	return IO::Maps::Basemap::newTileFromPBFData(level, tileIndexPair, buffer.data(), buffer.size());

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(tileIndexPair);

#endif

	ocean_assert(false && "Current workaround, use tigon-based function instead");

	return nullptr;
}

}

}
