cmake_minimum_required(VERSION 3.12)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(tracker_companion VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(JUCE)

juce_add_gui_app(companion
    ICON_BIG "logo.png"
    ICON_SMALL "logo.png"
    COMPANY_NAME "Spatial Media Lab"
    PRODUCT_NAME "tracker companion app")

juce_generate_juce_header(companion)

target_sources(companion
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/MainComponent.h

        Source/destinations/Destination.h
        Source/destinations/Visualizer.h
        Source/destinations/OSCSender.h
        Source/destinations/OSCSenderPlus.h

        Source/tracker/AddButton.h
        Source/tracker/Connection.h
        Source/tracker/ConnectionComponent.h
        Source/tracker/ConnectionManager.h
        Source/tracker/TrackerInterface.cpp
        Source/tracker/TrackerInterface.h
        Source/tracker/TrackerInterfaceComponent.h
        Source/tracker/Quaternion.h
        Source/tracker/RemoveButton.h
        Source/tracker/RotationData.h
        Source/tracker/MidiDeviceManager.h
        )


target_compile_definitions(companion
    PRIVATE
        DONT_SET_USING_JUCE_NAMESPACE=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:companion,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:companion,JUCE_VERSION>")

target_link_libraries(companion
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_devices
        juce::juce_osc

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)

set_property (TARGET companion PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
