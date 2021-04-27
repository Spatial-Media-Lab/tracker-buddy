cmake_minimum_required(VERSION 3.12)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(tracker_companion VERSION 0.0.1)

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

        Source/utilities/TrackerInterface.cpp
        Source/utilities/TrackerInterface.h
        Source/utilities/TrackerInterfaceComponent.h
        Source/utilities/Quaternion.h
        Source/utilities/TaitBryan.h
        Source/utilities/MidiDeviceManager.h
        Source/utilities/MidiDeviceManagerComponent.h
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
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags)
