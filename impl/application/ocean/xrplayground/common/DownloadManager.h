// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_DOWNLOAD_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_DOWNLOAD_MANAGER_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/network/tigon/TigonClient.h"

#include <future>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the manager for data that can be downloaded.
 * The manager allows to download data associated with an asset id.<br>
 * The manager allows to download data associated with an CDN url.<br>
 * The manager stores downloaded content on disk to ensure that the same asset id does not need to be downloaded every time it is accessed.
 * @ingroup xrplayground
 */
class DownloadManager :
	public Singleton<DownloadManager>,
	protected Thread
{
	friend class Singleton<DownloadManager>;

	protected:

		/**
		 * Re-definition of a TigonResponse object.
		 */
		using TigonResponse = Network::Tigon::TigonRequest::TigonResponse;

		/**
		 * This class holds the relevant data for a GraphQL query and http get request for an asset id.
		 */
		class ResponseData
		{
			public:

				/**
				 * Creates a new response object.
				 * @param assetId The id of the asset for which this object will hold the response, must be valid
				 * @param responseFuture The future holding the Tigon response, must be valid
				 * @param filePromise The promise for the downloaded file, must be valid
				 */
				inline ResponseData(std::string&& assetId, std::future<TigonResponse>&& responseFuture, std::promise<IO::File>&& filePromise);

				/**
				 * Creates a new response object.
				 * @param assetId The id of the asset for which this object will hold the response, must be valid
				 * @param filename The filename of the asset id, must be valid
				 * @param responseFuture The future holding the Tigon response, must be valid
				 * @param filePromise The promise for the downloaded file, must be valid
				 */
				inline ResponseData(std::string&& assetId, std::string&& filename, std::future<TigonResponse>&& responseFuture, std::promise<IO::File>&& filePromise);

			public:

				/// The id of the asset for which this object is holding the response.
				std::string assetId_;

				/// The filename of the asset.
				std::string filename_;

				/// The future holding the Tigon response.
				std::future<TigonResponse> responseFuture_;

				/// The promise for the downloaded file.
				std::promise<IO::File> filePromise_;
		};

		/**
		 * Definition of a vector holding response data objects.
		 */
		typedef std::vector<ResponseData> ResponseDatas;

	public:

		/**
		 * Downloads a new asset id.
		 * In case the asset has been downloaded before (and still exists on disk), the downloaded file will be returned imediately.
		 * @param assetId The id of the asset to download
		 * @return The future containing the file of the downloaded asset, invalid if failed
		 */
		std::future<IO::File> downloadAsset(const std::string& assetId);

		/**
		 * Downloads a new asset with CDN url
		 * In case the asset has been downloaded before (and still exists on disk), the downloaded file will be returned imediately.
		 * @param assetId The id of the asset, must be valid
		 * @param url CDN url for asset to download, must be valid
		 * @return The future containing the file of the downloaded asset, invalid if failed
		 */
		std::future<IO::File> downloadAssetFromMetaCdnUrl(const std::string& assetId, const std::string& url);

	protected:

		/**
		 * Protected default constructor.
		 */
		DownloadManager();

		/**
		 * Destructs the object.
		 */
		~DownloadManager() override;

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Invokes a new file request for a given asset id.
		 * @param assetId The asset id for which the downloaded file will be returned
		 * @return The resulting future containing the downloaded file, invalid if failed
		 */
		std::future<IO::File> invokeFileRequest(const std::string& assetId);

		/**
		 * Handles the pending GraphQL responses.
		 * The GraphQL responses translate the asset ids to http urls.
		 */
		void handlePendingGraphQLResponses();

		/**
		 * Handles the pending http get download responses.
		 */
		void handlePendingDownloadResponses();

		/**
		 * Cleans all downloaded asset folders which are too old.
		 */
		void cleanAssetFolder();

		/**
		 * Extracts the http url from a response for a GraphQL 'CreateXRAssetAction' query.
		 * @param tigonResponse The valid and succeeded Tigon response holding the response, must be valid
		 * @param url The resulting url
		 * @param filename The filename associated with the url
		 * @return True, if succeeded
		 */
		static bool extractURLFromRequest(const TigonResponse& tigonResponse, std::string& url, std::string& filename);

		/**
		 * Returns the root directory in which the downloaded data is stored.
		 * @return The root directory
		 */
		static IO::Directory rootDirectory();

		/**
		 * Writes a file with name 'timestamp' to a given directory containing the current timestamp.
		 * @param directory The directory in which the file will be written, must be valid, must exist
		 */
		static void writeTimestamp(const IO::Directory& directory);

		/**
		 * Extract filename from the Tigon response
		 * @param response Tigon response from a network request
		 * @return The extracted filename from response or an empty string on failure.
		 */
		static std::string extractFileNameFromTigonResponse(const TigonResponse& response);

	protected:

		/// The root directory in which the downloaded data will be stored.
		const IO::Directory rootDirectory_;

		/// The pending responses for GraphQL queries.
		ResponseDatas pendingGraphQLResponses_;

		/// The pending responses for http get requests.
		ResponseDatas pendingDownloadResponses_;

		/// The manager's lock.
		Lock lock_;
};

inline DownloadManager::ResponseData::ResponseData(std::string&& assetId, std::future<TigonResponse>&& responseFuture, std::promise<IO::File>&& filePromise) :
	assetId_(std::move(assetId)),
	responseFuture_(std::move(responseFuture)),
	filePromise_(std::move(filePromise))
{
	ocean_assert(!assetId_.empty());
	ocean_assert(responseFuture_.valid());
}

inline DownloadManager::ResponseData::ResponseData(std::string&& assetId, std::string&& filename, std::future<TigonResponse>&& responseFuture, std::promise<IO::File>&& filePromise) :
	assetId_(std::move(assetId)),
	filename_(std::move(filename)),
	responseFuture_(std::move(responseFuture)),
	filePromise_(std::move(filePromise))
{
	ocean_assert(!assetId_.empty());
	ocean_assert(!filename_.empty());
	ocean_assert(responseFuture_.valid());
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_DOWNLOAD_MANAGER_H
