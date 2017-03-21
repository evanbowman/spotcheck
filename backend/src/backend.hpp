//! @file
//! @brief Backend Module

#pragma once

#include "async.hpp"
#include "make_cv_roi.hpp"
#include "preview_normalized.hpp"
#include "results.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <node.h>
#include <node_object_wrap.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <stack>
#include <string>
#include <tuple>
#include <utility>
#include <v8.h>

class backend : public node::ObjectWrap {
public:
    //! @brief Initializes the backend module
    //!
    //! This function is part of the backend javascript API. It is responsible
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

    //! @brief Allocates a new backend object
    //!
    //! This function is part of the backend javascript API.
    //! It is called upon allocation of a new backend object by the
    //! frontend, and is responsible for performing the initial memory
    //! allocation.
    //!
    //! @return A backend javascript object.
    static void alloc(const callback_info & args);

    //! @brief Imports the source image into the backend
    //! 
    //! This function is part of the backend javascript API. It takes 
    //! two javascript objects as parameters; a path to the image
    //! to be loaded and a callback to execute upon completion.
    static void import_source_image(const callback_info & args);

    //! @brief Imports the source gal into the backend
    //!
    //! This function is part of the backend javascript API. It takes
    //! two javascript objects as parameters; a path to the gal file
    //! to be loaded and a callback to execute upon completion.
    //!
    //! @deprecated The frontend now handles gal data imports.
    [[deprecated]] static void import_source_gal(const callback_info & args);

    //! @brief Splits the backend's heightmap into smaller chunks
    //!
    //! This function is part of the backend javascript API. It splits
    //! the source heightmap into smaller subimages based on the list of
    //! targets previously specified by calling backend.add_target(...).
    //! After processing each sector, it writes three images to the
    //! directory frontend/temp/: the original segmented image,
    //! a binary mask representing where the microarray droplet might
    //! be located within the image, and a normalized image for easier
    //! viewing within the frontend.
    static void split_sectors(const callback_info & args);

    //! @brief Clears the backend's analysis targets
    //!
    //! This function is part of the backend javascript API. It clears
    //! out the list of analysis targets maintained by the backend.
    static void clear_targets(const callback_info & args);

    //! @brief Launches analysis on all of the backend's analysis targets
    //!
    //! This function is part of the backend javascript API. It
    //! launches an analysis run, which submits an asynchronous task
    //! for each analysis target, each of which calls backend::analyze_target
    //! on the dataset for that target.
    static void launch_analysis(const callback_info & args);

    //! @brief Runs analysis on a single analysis target
    //!
    //! This function runs the full suite of analysis metrics on
    //! a subregion of the heightmap. Upon completion, it writes a result
    //! object to the backend's list of results.
    //!
    //! @param target The target struct with metadata about the target subregion
    //! @param src The original source image representing the subregion
    //! @param mask A binary mask of the subregion representing droplet location
    static void analyze_target(Target & target, cv::Mat & src, cv::Mat & mask);

    //! @brief Adds an analysis target to the backend
    //!
    //! This function is part of the backend javascript API. It adds an analysis
    //! target to the backend, where each target is metadata that describes the
    //! subregion of the image to be analysed.
    //! From javascript, it takes seven parameters:
    //! 1) rowId: The row of the selection grid where the subimage lies.
    //! 2) colId: The column of the selection grid where the subimage lies.
    //! 3) fractStartx: The x percentage into the image where the subimage begins.
    //! 4) fractStarty: The y percentage into the image where the subimage begins.
    //! 5) fractEndx: The x percentage into the image where the subimage ends.
    //! 6) fractEndy: The y percentage into the image where the subimage ends.
    //! 7) threshold: A default threshold value (unused)
    static void add_target(const callback_info & args);

    //! @brief Gets the threshold value for a target
    //!
    //! This function is part of the backend javascript API. It takes two
    //! parameters, the target row followed by the target column of the
    //! desired subimage to the current threshold value for.
    static void get_target_thresh(const callback_info & args);

    //! @brief Serialize the backend's results to a JSON format
    //!
    //! This function is part of the backend javascript API. It takes
    //! one parameter, a javascript callback function to be called when
    //! serialization finishes. The backend writes results to the path
    //! frontend/temp/results.json.
    static void write_results_JSON(const callback_info & args);

    //! @brief Updates the threshold value for an analysis target
    //!
    //! This function is part of the backend javascript API. It takes
    //! four parameters; a callback, followed by the row and column describing
    //! which target to modify, then the substitute threshold value.
    static void update_target_thresh(const callback_info & args);

    //! @brief Provides a preview of the entire heighmap
    //!
    //! This function is part of the backend javascript API. It takes one
    //! parameter, a callback to be executed upon completion.
    static void provide_norm_preview(const callback_info & args);

    //! @brief Queries whether the backend is working on analysis targets
    //!
    //! This function is part of the backend javascript API. It returns true
    //! if the backend has no more analysis tasks to complete.
    static void is_busy(const callback_info & args);
    
private:
    static std::vector<result> m_results;
    static cv::Mat m_source_image;
    static std::vector<Target> m_targets;
};
