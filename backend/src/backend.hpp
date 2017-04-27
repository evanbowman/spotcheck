//! @file
//! @brief Backend Module

#pragma once

#include "async.hpp"
#include "rapidjson/document.h"
#include "make_cv_roi.hpp"
#include "preview_normalized.hpp"
#include "results.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <node.h>
#include <node_object_wrap.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <utility>
#include <v8.h>
// TODO: integrate yaml-cpp into windows builds
// #include <yaml-cpp/yaml.h>

//! @brief The interface between Spotcheck's analysis functions, OpenCV, and
//! the application's frontend.
//!
//! The Backend class provides an interface for passing data between the app's
//! frontend and SpotCheck's image analysis functions as described in
//! analysis.hpp. It does so by defining the mapping between it's own class
//! methods and javascript functions, so that a Backend object can be
//! instantiated directly in javascript! To better understand how the dispatch
//! between a Backend object's methods and javascript functions works, see the
//! implementation of Backend::init in Backend.cpp. The frontend begins by
//! telling
//! the Backend to load a tiff heightmap with Backend::import_source_image, and
//! then by describing what subregions of the image to analyze by calling
//! Backend::add_target for each square of the frontend's selection grid. After
//! specifying all the targets to analyze, the frontend calls
//! Backend::launch_analysis, which submits a task to the app's threadpool for
//! each analysis target. Each task runs Backend::analyze_target on a single
//! target, and upon completion, writes a result object containing the analysis
//! results to Backend::m_results. The frontend can then call
//! Backend::write_results_JSON to import the results data as an array of
//! javascript objects.
class Backend : public node::ObjectWrap {
public:
    //! @brief Initializes the Backend module
    //!
    //! This function is part of the Backend javascript API. It is responsible
    //! for performing the mapping between this class' static members
    //! and the javascript API functions accessible from the frontend.
    static void init(v8::Local<v8::Object> exports,
                     v8::Local<v8::Object> module);
    struct Target {
        int64_t rowId, colId;
        double fractStartx, fractStarty;
        double fractEndx, fractEndy;
        int64_t threshold;
    };
    using callback_info = v8::FunctionCallbackInfo<v8::Value>;
    static v8::Persistent<v8::Function> constructor;

    //! @brief Allocates a new Backend object
    //!
    //! This function is part of the Backend javascript API.
    //! It is called upon allocation of a new Backend object by the
    //! frontend, and is responsible for performing the initial memory
    //! allocation.
    //!
    //! @return A Backend javascript object.
    static void alloc(const callback_info & args);

    //! @brief Imports the source image into the Backend
    //!
    //! This function is part of the Backend javascript API. It takes
    //! two javascript objects as parameters; a path to the image
    //! to be loaded and a callback to execute upon completion.
    static void import_source_image(const callback_info & args);

    //! @brief Splits the Backend's heightmap into smaller chunks
    //!
    //! This function is part of the Backend javascript API. It splits
    //! the source heightmap into smaller subimages based on the list of
    //! targets previously specified by calling Backend.add_target(...).
    //! After processing each sector, it writes three images to the
    //! directory frontend/temp/: the original segmented image,
    //! a binary mask representing where the microarray droplet might
    //! be located within the image, and a normalized image for easier
    //! viewing within the frontend.
    static void split_sectors(const callback_info & args);

    //! @brief Sets the pixel width for unit scaling
	//!

    static void set_pixel_width(const callback_info & args);

	//! @brief Sets the pixel depth for unit scaling
	//!

	static void set_pixel_depth(const callback_info & args);
    
    //! @brief Clears the Backend's analysis targets
    //!
    //! This function is part of the Backend javascript API. It clears
    //! out the list of analysis targets maintained by the Backend.
    static void clear_targets(const callback_info & args);

    //! @brief Launches analysis on all of the Backend's analysis targets
    //!
    //! This function is part of the Backend javascript API. It
    //! launches an analysis run, which submits an asynchronous task
    //! for each analysis target, each of which calls Backend::analyze_target
    //! on the dataset for that target.
    static void launch_analysis(const callback_info & args);

    //! @brief Runs analysis on a single analysis target
    //!
    //! This function runs the full suite of analysis metrics on
    //! a subregion of the heightmap. Upon completion, it writes a result
    //! object to the Backend's list of results.
    //!
    //! @param target The target struct with metadata about the target subregion
    //! @param src The original source image representing the subregion
    //! @param mask A binary mask of the subregion representing droplet location
    static void analyze_target(Target & target, cv::Mat & src, cv::Mat & mask);

    //! @brief Adds an analysis target to the Backend
    //!
    //! This function is part of the Backend javascript API. It adds an analysis
    //! target to the Backend, where each target is metadata that describes the
    //! subregion of the image to be analysed.
    //! From javascript, it takes seven parameters:
    //! 1) rowId: The row of the selection grid where the subimage lies.
    //! 2) colId: The column of the selection grid where the subimage lies.
    //! 3) fractStartx: The x percentage into the image where the subimage
    //! begins.
    //! 4) fractStarty: The y percentage into the image where the subimage
    //! begins.
    //! 5) fractEndx: The x percentage into the image where the subimage ends.
    //! 6) fractEndy: The y percentage into the image where the subimage ends.
    //! 7) threshold: A default threshold value (unused)
    static void add_target(const callback_info & args);

    //! @brief Gets the threshold value for a target
    //!
    //! This function is part of the Backend javascript API. It takes two
    //! parameters, the target row followed by the target column of the
    //! desired subimage to the current threshold value for.
    static void get_target_thresh(const callback_info & args);

    //! @brief Serialize the Backend's results to a JSON format
    //!
    //! This function is part of the Backend javascript API. It takes
    //! one parameter, a javascript callback function to be called when
    //! serialization finishes. The Backend writes results to the path
    //! frontend/temp/results.json.
    static void write_results_JSON(const callback_info & args);

    //! @brief Updates the threshold value for an analysis target
    //!
    //! This function is part of the Backend javascript API. It takes
    //! four parameters; a callback, followed by the row and column describing
    //! which target to modify, then the substitute threshold value.
    static void update_target_thresh(const callback_info & args);

    //! @brief Provides a preview of the entire heighmap
    //!
    //! This function is part of the Backend javascript API. It takes one
    //! parameter, a callback to be executed upon completion.
    static void provide_norm_preview(const callback_info & args);

    //! @brief Queries whether the Backend is working on analysis targets
    //!
    //! This function is part of the Backend javascript API. It returns true
    //! if the Backend has no more analysis tasks to complete.
    static void is_busy(const callback_info & args);

    //! @brief Configures backend test suite based on user config file.
    //!
    //! Responsible for populating the backend's table of enabled builtin
    //! functions, as well as user defined metrics. It compiles each user
    //! metric script and stores them internally for use when running analysis.
    //!
    //! This function is part of the Backend javascript API. It takes as a
    //! parameter the path to load the config file from.
    static void configure(const callback_info & args);

    //! @brief Writes a default config file if the user does not have one.
    //!
    //! A default config file contains all of the builtin metrics available
    //! to users.
    static void write_default_config(const callback_info & args);

private:
    static std::map<std::pair<int64_t, int64_t>, Result> m_results;
    static cv::Mat m_source_image;
    static std::vector<Target> m_targets;
    static std::set<std::string> m_enabled_builtins;
    static std::map<std::string, std::string> m_usr_scripts;
    static double m_pixel_width;
	static double m_pixel_depth;

    //! @brief Compiles each user script and runs it on all targets.
    //!
    //! To be safe, be sure to call this function in launch_analysis
    //! PRIOR to the loop that launches the asynchronous builtin
    //! analysis functions. This will ensure that all the data gets
    //! written before the frontend requests the results.
    //!
    //! All of the user metrics run in a sandboxed, isolated context,
    //! where there should be no risk of injection attacks. Eval is also
    //! disabled.
    static void run_user_metrics();
};
