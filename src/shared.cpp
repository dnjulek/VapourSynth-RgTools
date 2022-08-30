#include "common.h"

VS_EXTERNAL_API(void) VapourSynthPluginInit2(VSPlugin* plugin, const VSPLUGINAPI* vspapi) {
	vspapi->configPlugin("com.julek.rgtools", "rgtools", "RgTools", VS_MAKE_VERSION(1, 0), VAPOURSYNTH_API_VERSION, 0, plugin);
	vspapi->registerFunction("RemoveGrain", "clip:vnode;mode:int:opt;", "clip:vnode;", rgToolsCreate, nullptr, plugin);
}