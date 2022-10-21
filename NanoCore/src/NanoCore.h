

//-----Application--------------
#include "core/base/Application.h"
//------------------------------

#include "modules/ref/Shared.h"

//-----Main---------------

//------------------------------


//-----log----------------------
#include "core/log/Log.h"
//------------------------------


//-----layer--------------------
#include "core/layer/Layer.h"
//------------------------------


//-----imgui--------------------
#include "modules/ui/UILayer.h"
//------------------------------

//-----input--------------------
#include "modules/events/Input.h"
#include "modules/events/EventCodes.h"
//------------------------------


//// ---Renderer------------------------
#include "modules/rendering/Renderer.h"
#include "modules/rendering/RenderCommand.h"

#include "modules/rendering/Buffer.h"
#include "modules/rendering/Shader.h"
#include "modules/rendering/VertexArray.h"

#include "modules/rendering/Texture.h"

#include "modules/entity/OrthographicCamera.h"
#include "modules/entity/OrthographicCameraController.h"
#include "modules/rendering/Framebuffer.h"

// utils.
#include "modules/utils/Instrumentor.h"
#include "modules/utils/RenderUtils.h"
#include "modules/utils/Random.h"

#include "modules/utils/PlatformUtils.h"
#include "core/base/Base.h"




#include "modules/entity/Entity.h"
#include "modules/entity/Components.h"
#include "modules/entity/Asset.h"
#include "modules/entity/SceneSerializer.h"

#include "core/math/NanoMath.h"
#include "modules/entity/EditorCamera.h"



#include "modules/utils/FileManager.h"
#include "modules/utils/Hash.h"

#include "modules/info/Project.h"


#include "modules/script/ScriptEngine.h"
#include "modules/script/ScriptGlue.h"