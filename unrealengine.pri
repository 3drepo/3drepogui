#  Copyright (C) 2015 3D Repo Ltd
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#-------------------------------------------------------------------------------
# Unreal Engine 4 integration
# See https://wiki.unrealengine.com/Using_QtCreator_With_UnrealEngine4
# For doxygen: https://forums.unrealengine.com/showthread.php?15803-UE4-C-Documentation-within-Qt-Creator
# API: https://docs.unrealengine.com/latest/INT/Programming/

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/submodules/UnrealEngine/Engine/Binaries/Win64/ -lUnrealHeaderTool-Core
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/submodules/UnrealEngine/Engine/Binaries/Win64/ -lUnrealHeaderTool-Core

win32:DEFINES += "UBT_COMPILED_PLATFORM=Win64"
win32:DEFINES += "WIN32=1"

# See https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745.aspx
win32:DEFINES += "_WIN32_WINNT=0x0502"
win32:DEFINES += "WINVER=0x0502" # Win Server 2003
win32:DEFINES += "PLATFORM_WINDOWS=1"

win32:INCLUDEPATH += "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/INCLUDE" \
                     "C:/Program Files (x86)/Microsoft Visual Studio 12.0/VC/ATLMFC/INCLUDE" \
                     "C:/Program Files (x86)/Windows Kits/8.1/include/shared" \
                     "C:/Program Files (x86)/Windows Kits/8.1/include/um" \
                     "C:/Program Files (x86)/Windows Kits/8.1/include/winrt"


#-------------------------------------------------------------------------------
DEFINES += "$(NMakePreprocessorDefinitions)"
DEFINES += "UE_EDITOR=1"
DEFINES += "IS_PROGRAM=0"
DEFINES += "UE_ROCKET=0"
DEFINES += "UNICODE"
DEFINES += "_UNICODE"
DEFINES += "__UNREAL__"
DEFINES += "IS_MONOLITHIC=0"
DEFINES += "WITH_ENGINE=1"
DEFINES += "WITH_UNREAL_DEVELOPER_TOOLS=0"
DEFINES += "WITH_COREUOBJECT=1"
DEFINES += "USE_STATS_WITHOUT_ENGINE=1"
DEFINES += "WITH_PLUGIN_SUPPORT=1"
DEFINES += "USE_LOGGING_IN_SHIPPING=1"
DEFINES += "UE_BUILD_MINIMAL=0"
DEFINES += "WITH_EDITOR=1"
DEFINES += "WITH_SERVER_CODE=1"
DEFINES += "NDEBUG=1"
DEFINES += "UE_BUILD_DEVELOPMENT=1"
#DEFINES += "ORIGINAL_FILE_NAME=UE4Editor-FP_FirstPerson.dll"
DEFINES += "DEPRECATED_FORGAME=DEPRECATED"
DEFINES += "FP_FIRSTPERSON_API="
DEFINES += "UE_ENABLE_ICU=1"
DEFINES += "WITH_STEAMWORKS=0"
DEFINES += "WITH_DIRECTXMATH=0"
DEFINES += "CORE_API="s
DEFINES += "COREUOBJECT_API="
DEFINES += "WITH_PHYSX=1"
DEFINES += "WITH_APEX=1"
DEFINES += "WITH_RUNTIME_PHYSICS_COOKING"
DEFINES += "WITH_BOX2D=1"
DEFINES += "WITH_RECAST=1"
DEFINES += "ENGINE_API="
DEFINES += "JSON_API="
DEFINES += "WITH_FREETYPE=1"
DEFINES += "SLATECORE_API="
DEFINES += "INPUTCORE_API="
DEFINES += "SLATE_API="
DEFINES += "MESSAGING_API="
DEFINES += "RENDERCORE_API="
DEFINES += "RHI_API="
DEFINES += "SHADERCORE_API="
DEFINES += "ASSETREGISTRY_API="
DEFINES += "ENGINEMESSAGES_API="
DEFINES += "ENGINESETTINGS_API="
DEFINES += "SYNTHBENCHMARK_API="
DEFINES += "RENDERER_API="
DEFINES += "AIMODULE_API="
DEFINES += "VECTORVM_API="
DEFINES += "DATABASESUPPORT_API="
DEFINES += "UNREALED_API="
DEFINES += "BSPMODE_API="
DEFINES += "DOCUMENTATION_API="
DEFINES += "PROJECTS_API="
DEFINES += "SANDBOXFILE_API="
DEFINES += "EDITORSTYLE_API="
DEFINES += "SOURCECONTROL_API="
DEFINES += "UNREALEDMESSAGES_API="
DEFINES += "BLUEPRINTGRAPH_API="
DEFINES += "HTTP_PACKAGE=1"
DEFINES += "HTTP_API="
DEFINES += "XAUDIO2_API="
DEFINES += "USERFEEDBACK_API="
DEFINES += "COLLECTIONMANAGER_API="
DEFINES += "NIAGARAEDITOR_API="
DEFINES += "GRAPHEDITOR_API="
DEFINES += "INTROTUTORIALS_API="
DEFINES += "LEVELEDITOR_API="
DEFINES += "SETTINGS_API="
DEFINES += "KISMET_API="

INCLUDEPATH += "$(NMakeIncludeSearchPath)"
#INCLUDEPATH += "../../Source"
#INCLUDEPATH += "../Build/Win64/Inc/FP_FirstPerson"
#INCLUDEPATH += "../../Source/FP_FirstPerson"
win32:INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Windows" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/CoreUObject" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Engine" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Json" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/SlateCore" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/InputCore" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Slate" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Messaging" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/RenderCore" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/RHI" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/ShaderCore" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/AssetRegistry" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/EngineMessages" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/EngineSettings" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/SynthBenchmark" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Renderer" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/AIModule" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/VectorVM" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/DatabaseSupport" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/UnrealEd" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/BspMode" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Documentation" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Projects" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/SandboxFile" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/EditorStyle" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/SourceControl" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/UnrealEdMessages" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/BlueprintGraph" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/HTTP" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/XAudio2" \
                     "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Windows/XAudio2/Public" \
                     "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Windows/XAudio2" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/UserFeedback" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/CollectionManager" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/NiagaraEditor" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/GraphEditor" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/IntroTutorials" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/LevelEditor" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Settings" \
                     "$$PWD/submodules/UnrealEngine/Engine/Intermediate/Build/Win64/Inc/Kismet"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Internationalization"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Async"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Concurrency"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Containers"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Delegates"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/GenericPlatform"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/HAL"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Logging"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Math"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Misc"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Modules"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Modules/Boilerplate"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/ProfilingDebugging"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Csv"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Stats"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/Templates"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core/Public/UObject"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Core"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public/Blueprint"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public/Misc"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public/Serialization"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public/Templates"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject/Public/UObject"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/CoreUObject"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/AI"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/EdGraph"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/Features"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/Net"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/Slate"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/Tests"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/VisualLogger"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine/Public/AI/Navigation"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Json/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Json/Public/Dom"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Json/Public/Policies"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Json/Public/Serialization"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Json"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Animation"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Application"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Brushes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Fonts"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Input"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Layout"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Logging"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Rendering"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Sound"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Styling"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Textures"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Types"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore/Public/Widgets"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/InputCore/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/InputCore/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/InputCore"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SlateCore"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Application"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Commands"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Docking"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Layout"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/MultiBox"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Notifications"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Styling"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Text"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Views"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Text/Android"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Framework/Text/IOS"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Colors"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Docking"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Images"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Input"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/LayerManager"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Layout"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Navigation"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Notifications"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Text"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate/Public/Widgets/Views"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Slate"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Messaging/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Messaging/Public/Deprecated"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Messaging/Public/Helpers"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Messaging"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/RenderCore/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/RenderCore"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/RHI/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/RHI"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/ShaderCore/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/ShaderCore"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/AssetRegistry/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/AssetRegistry"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineMessages/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineMessages/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineMessages"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineSettings/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineSettings/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/EngineSettings"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/SynthBenchmark/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Renderer/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Renderer"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/SynthBenchmark"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/AIModule/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/AIModule/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/AIModule"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/VectorVM/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/VectorVM"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/DatabaseSupport/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/DatabaseSupport"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Programs/UnrealLightmass/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/AutoReimport"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Commandlets"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Dialogs"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/DragAndDrop"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/FeaturePack"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Features"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Kismet2"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Layers"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Tests"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd/Public/Toolkits"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/BspMode/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/BspMode"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Documentation/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Documentation"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Projects/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Projects/Public/Interfaces"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Projects"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SandboxFile/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/SandboxFile"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/EditorStyle/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/EditorStyle/Public/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/EditorStyle/Public/Interfaces"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/EditorStyle"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/SourceControl/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/SourceControl/Public/Tests"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/SourceControl"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEdMessages/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEdMessages/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEdMessages"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/BlueprintGraph/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/BlueprintGraph/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/BlueprintGraph"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Online/HTTP/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Online/HTTP/Public/Interfaces"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Online/HTTP"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UserFeedback/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UserFeedback"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/CollectionManager/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/CollectionManager"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/NiagaraEditor/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/GraphEditor/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/IntroTutorials/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/IntroTutorials"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/GraphEditor"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/LevelEditor/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/Settings/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Developer/Settings"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/LevelEditor"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/NiagaraEditor"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/UnrealEd"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Kismet/Classes"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Kismet/Public"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Kismet/Public/WorkflowOrientedApp"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Editor/Kismet"
INCLUDEPATH += "$$PWD/submodules/UnrealEngine/Engine/Source/Runtime/Engine"
