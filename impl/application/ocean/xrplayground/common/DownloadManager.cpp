// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/DownloadManager.h"

#include "ocean/base/String.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/JSONConfig.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/platform/android/Resource.h"
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE
	#include "ocean/platform/apple/Utilities.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

DownloadManager::DownloadManager() :
	rootDirectory_(rootDirectory())
{
	// nothing to do here
}

DownloadManager::~DownloadManager()
{
	stopThreadExplicitly();
}

std::future<IO::File> DownloadManager::downloadAsset(const std::string& assetId)
{
	ocean_assert(assetId == String::trim(assetId));

	if (!rootDirectory_.exists())
	{
		rootDirectory_.create();
	}

	const IO::Directory dataDirectory = rootDirectory_ + IO::Directory(assetId);

	if (dataDirectory.exists())
	{
		const IO::Files files = dataDirectory.findFiles();

		for (const IO::File& file : files)
		{
			if (file.name() != "timestamp")
			{
				writeTimestamp(IO::Directory(file));

				std::promise<IO::File> filePromise;
				filePromise.set_value(file);

				return filePromise.get_future();
			}
		}

		ocean_assert(false && "File does not exist anymore, this should never happen");
	}

	std::future<IO::File> fileFuture = invokeFileRequest(assetId);

	if (fileFuture.valid())
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return fileFuture;
}

std::future<IO::File> DownloadManager::downloadAssetFromMetaCdnUrl(const std::string& assetId, const std::string& url) {
	ocean_assert(!assetId.empty());
	ocean_assert(!url.empty() && url == String::trim(url));

	if (!rootDirectory_.exists())
	{
		rootDirectory_.create();
	}

	const IO::Directory dataDirectory = rootDirectory_ + IO::Directory(assetId);

	if (dataDirectory.exists())
	{
		const IO::Files files = dataDirectory.findFiles();

		for (const IO::File& file : files)
		{
			if (file.name() != "timestamp")
			{
				writeTimestamp(IO::Directory(file));

				std::promise<IO::File> filePromise;
				filePromise.set_value(file);

				return filePromise.get_future();
			}
		}
	}

	std::promise<IO::File> filePromise;
	std::future<IO::File> fileFuture = filePromise.get_future();

	std::future<TigonResponse> futureResponse = Network::Tigon::TigonClient::get().httpRequest(url);

	if (futureResponse.valid())
	{
	    TemporaryScopedLock scopedLock(lock_);

	    pendingDownloadResponses_.emplace_back(std::string(assetId), std::string(assetId), std::move(futureResponse), std::move(filePromise));
	}
	else
	{
		Log::error() << "Failed to invoke http request for asset id " << assetId << " for url " << url;
	}

	if (fileFuture.valid())
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return fileFuture;
}

void DownloadManager::threadRun()
{
	bool assetFolderCleaned = false;

	while (!shouldThreadStop())
	{
		sleep(1u);

		handlePendingGraphQLResponses();

		handlePendingDownloadResponses();

		if (!assetFolderCleaned)
		{
			cleanAssetFolder();
			assetFolderCleaned = true;
		}
	}
}

std::future<IO::File> DownloadManager::invokeFileRequest(const std::string& assetId)
{
	// we invoke the QueryXRAssetCDNUrl GraphQL query to determine the url for the given asset id

	/**
	 * query QueryXRAssetCDNUrl($xr_asset_id: ID!)
	 * {
	 *     xfb_xr_asset_cdn_url_query(xr_asset_id: $xr_asset_id)
	 *     {
	 *         cdn_url,
	 *         file_name
	 *      }
	 * }
	 *
	 * persist id: 5063122860414988
	 */

	const std::string query = "5063122860414988";
	constexpr bool queryIsPersistId = true;

	const std::string parameters = "{ \"xr_asset_id\": " + assetId + " }";

	std::future<TigonResponse> futureResponse = Network::Tigon::TigonClient::get().graphQLRequest(query, queryIsPersistId, parameters);

	if (!futureResponse.valid())
	{
		return std::future<IO::File>();
	}

	const ScopedLock scopedLock(lock_);

	std::promise<IO::File> filePromise;
	std::future<IO::File> fileFuture = filePromise.get_future();

	pendingGraphQLResponses_.emplace_back(std::string(assetId), std::move(futureResponse), std::move(filePromise));

	return fileFuture;
}

void DownloadManager::handlePendingGraphQLResponses()
{
	TemporaryScopedLock scopedLock(lock_);

	for (size_t nResponse = 0; nResponse < pendingGraphQLResponses_.size(); ++nResponse)
	{
		ocean_assert(pendingGraphQLResponses_[nResponse].responseFuture_.valid());

		if (pendingGraphQLResponses_[nResponse].responseFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			ResponseData responseData = std::move(pendingGraphQLResponses_[nResponse]);

			pendingGraphQLResponses_[nResponse] = std::move(pendingGraphQLResponses_.back());
			pendingGraphQLResponses_.pop_back();

			scopedLock.release();

			const TigonResponse tigonResponse = responseData.responseFuture_.get();
			std::promise<IO::File> filePromise = std::move(responseData.filePromise_);

			if (tigonResponse.succeeded())
			{
				std::string url;
				std::string filename;

				if (extractURLFromRequest(tigonResponse, url, filename))
				{
					std::future<TigonResponse> futureResponse = Network::Tigon::TigonClient::get().httpRequest(url);

					if (futureResponse.valid())
					{
						scopedLock.relock(lock_);

						pendingDownloadResponses_.emplace_back(std::move(responseData.assetId_), std::move(filename), std::move(futureResponse), std::move(filePromise));
						return;
					}
					else
					{
						Log::error() << "Failed to invoke http request for asset id " << responseData.assetId_ << " for url " << url;
					}
				}
				else
				{
					Log::error() << "Failed to determine url for asset id " << responseData.assetId_;

					ocean_assert(false && "This should never happen!");
				}
			}
			else
			{
				Log::error() << "Failed to determine url for asset id " << responseData.assetId_ << " with error " << tigonResponse.error();
			}

			filePromise.set_value(IO::File());

			// we do not handle more than one valid response per function call
			return;
		}
	}
}

void DownloadManager::handlePendingDownloadResponses()
{
	TemporaryScopedLock scopedLock(lock_);

	for (size_t nResponse = 0; nResponse < pendingDownloadResponses_.size(); ++nResponse)
	{
		ocean_assert(pendingDownloadResponses_[nResponse].responseFuture_.valid());

		if (pendingDownloadResponses_[nResponse].responseFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			ResponseData responseData = std::move(pendingDownloadResponses_[nResponse]);

			pendingDownloadResponses_[nResponse] = std::move(pendingDownloadResponses_.back());
			pendingDownloadResponses_.pop_back();

			scopedLock.release();

			const TigonResponse tigonResponse = responseData.responseFuture_.get();
			std::promise<IO::File> filePromise = std::move(responseData.filePromise_);

			IO::File assetFile;

			if (tigonResponse.succeeded())
			{
				const IO::Directory assetDirectory = rootDirectory_ + IO::Directory(responseData.assetId_);
				const std::string filename = extractFileNameFromTigonResponse(tigonResponse);

				assert(!assetDirectory.exists());
				if (assetDirectory.create())
				{
					writeTimestamp(assetDirectory);

					assetFile = assetDirectory + IO::File(filename.empty() ? responseData.filename_ : filename);
					std::ofstream stream(assetFile().c_str(), std::ios::binary);

					stream.write(tigonResponse.response().c_str(), tigonResponse.response().size());
				}
				else
				{
					Log::error() << "Failed to create asset directory '" << assetDirectory() << "'";
				}
			}
			else
			{
				Log::error() << "Failed to download asset id " << responseData.assetId_ << " with error " << tigonResponse.error();
			}

			filePromise.set_value(assetFile);

			// we do not handle more than one valid response per function call
			return;
		}
	}
}

void DownloadManager::cleanAssetFolder()
{
	const IO::Directories assetDirectories = rootDirectory_.findDirectories();

	const Timestamp currentTimestamp(true);

	for (const IO::Directory& assetDirectory : assetDirectories)
	{
		const IO::Files files = assetDirectory.findFiles();

		for (const IO::File& file : files)
		{
			if (file.name() == "timestamp")
			{
				std::ifstream stream(file().c_str(), std::ios::binary);

				stream.seekg (0, stream.end);
				const std::streampos length = stream.tellg();
				stream.seekg (0, stream.beg);

				if (length == sizeof(Timestamp))
				{
					Timestamp timestamp(false);
					stream.read((char*)(&timestamp), sizeof(timestamp));

					if (stream.good() && timestamp.isValid())
					{
						if (currentTimestamp >= timestamp + 86400.0 * 14.0) // 14 days
						{
							// let's remove the directory

							assetDirectory.remove(true /*recursively*/);
							break;
						}
					}
				}
			}
		}
	}
}

bool DownloadManager::extractURLFromRequest(const TigonResponse& tigonResponse, std::string& url, std::string& filename)
{
	ocean_assert(tigonResponse.succeeded());

	IO::JSONConfig config(std::shared_ptr<std::istream>(new std::istringstream(tigonResponse.response())));

	if (config.exist("data"))
	{
		IO::JSONConfig::JSONValue& urlQuery = config["data"]["xfb_xr_asset_cdn_url_query"];

		std::string responseURL = urlQuery["cdn_url"](std::string());
		std::string responseFilename = urlQuery["file_name"](std::string());

		responseURL = String::replace(std::move(responseURL), "\\/", "/", false);

		if (!responseURL.empty() && responseURL.find("https://") == 0 && !responseFilename.empty())
		{
			url = std::move(responseURL);
			filename = std::move(responseFilename);

			return true;
		}
	}

	return false;
}

IO::Directory DownloadManager::rootDirectory()
{
	IO::Directory directory;

#if defined(OCEAN_PLATFORM_BUILD_APPLE)
	directory = IO::Directory(Platform::Apple::Utilities::documentDirectory());
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	directory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("downloaded");
#endif

	ocean_assert(directory.isValid());

	return directory;
}

void DownloadManager::writeTimestamp(const IO::Directory& directory)
{
	ocean_assert(directory.isValid());

	const IO::File timestampFile(directory + IO::File("timestamp"));

	std::ofstream stream(timestampFile(), std::ios::binary);

	const Timestamp currentTimestamp(true);
	stream.write((const char*)(&currentTimestamp), sizeof(currentTimestamp));
	stream.close();
}

std::string DownloadManager::extractFileNameFromTigonResponse(const TigonResponse& response)
{
	std::string header;

	if (response.succeeded() && response.findHeader("Content-Disposition", header))
	{
		if (header.find("attachment") != std::string::npos)
		{
			const std::size_t filenamePos = header.find("filename=");

			if (filenamePos != std::string::npos)
			{
				return header.substr(filenamePos);
			}
		}
	}

	return "";
}

}

}
