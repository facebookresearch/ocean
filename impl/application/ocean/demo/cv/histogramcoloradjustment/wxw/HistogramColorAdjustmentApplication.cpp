// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/histogramcoloradjustment/wxw/HistogramColorAdjustmentApplication.h"
#include "application/ocean/demo/cv/histogramcoloradjustment/wxw/HistogramColorAdjustmentMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

#include "ocean/platform/wxwidgets/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#else
	#include "ocean/base/PluginManager.h"
#endif

// WxWidgets specific implementation of e.g. the application entry function
IMPLEMENT_APP(HistogramColorAdjustmentApplication)

bool HistogramColorAdjustmentApplication::OnInit()
{
	Messenger::get().setFileOutput("democvhistogramcoloradjustment_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

	const std::string frameworkPath(Platform::WxWidgets::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	HistogramColorAdjustmentMainWindow* mainWindow = new HistogramColorAdjustmentMainWindow((std::wstring(L"Histogram Color Adjustment, ") + Ocean::String::toWString(Ocean::Build::buildString())).c_str(), wxPoint(50, 50), wxSize(800, 600));
	mainWindow->Show(true);
	SetTopWindow(mainWindow);

	return true;
}

void HistogramColorAdjustmentApplication::CleanUp()
{

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

}
