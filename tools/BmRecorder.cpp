
// TODO:   Reduce the DRY

#include <string>
#include <thread>
using namespace std;

#include <signal.h>

#include <opencv2/opencv.hpp>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <tclap/CmdLine.h>

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "libvideoio/G3LogSinks.h"

#include "libvideoio/Display.h"
using namespace libvideoio;




bool keepGoing = true;

void signal_handler( int sig )
{
	switch( sig ) {
		case SIGINT:
		keepGoing = false;
		break;
	}
}

using cv::Mat;

int main( int argc, char** argv )
{
	auto worker = g3::LogWorker::createLogWorker();
	auto stderrHandle = worker->addSink(std::unique_ptr<ColorStderrSink>( new ColorStderrSink ),
	&ColorStderrSink::ReceiveLogMessage);
	g3::initializeLogging(worker.get());

	signal( SIGINT, signal_handler );

	//try {
	TCLAP::CmdLine cmd("BmRecorder", ' ', "0.1");

	// TCLAP::ValueArg<std::string> resolutionArg("r","resolution","Input resolution: hd2k,hd1080,hd720,vga",false,"hd1080","", cmd);
	// TCLAP::ValueArg<float> fpsArg("f","fps","Input FPS, otherwise defaults to max FPS from input source",false,0.0,"", cmd);
	//
	// TCLAP::ValueArg<std::string> svoOutputArg("s","svo-output","Output SVO file",false,"","", cmd);
	//
	// TCLAP::ValueArg<std::string> calibOutputArg("","calib-output","Output calibration file (from stereolabs SDK)",false,"","Calib filename", cmd);
	//
	// TCLAP::ValueArg<std::string> statisticsOutputArg("","statistics-output","",false,"","", cmd);
	// TCLAP::ValueArg<std::string> statisticsIdArg("","statistics-id","",false,"","", cmd);
	//
	//
	// // TCLAP::SwitchArg noGuiSwitch("","no-gui","Don't show a GUI", cmd, false);
	//
	// TCLAP::SwitchArg disableSelfCalibSwitch("","disable-self-calib","", cmd, false);
	//
	// TCLAP::SwitchArg guiSwitch("","display","", cmd, false);
	//
	//
	// TCLAP::ValueArg<int> durationArg("","duration","Duration",false,0,"seconds", cmd);
	// TCLAP::ValueArg<int> numFramesArg("","frames","Number of frames to capture",false,0,"frames", cmd);
	// TCLAP::ValueArg<int> skipArg("","skip","NOnly display every <skip> frames",false,10,"frames", cmd);
	//
	//
	//
	// cmd.parse(argc, argv );
	//
	// // Output validation
	// if( !svoOutputArg.isSet() && !guiSwitch.isSet() ) {
	// 	LOG(WARNING) << "No output options set.";
	// }
	//
	// const bool doDisplay = guiSwitch.getValue();
	// libvideoio::Display display( guiSwitch.getValue() );
	//
	// int skip = skipArg.getValue();
	//
	// //} catch (TCLAP::ArgException &e)  // catch any exceptions
	// //	{
	// //		std::cout<< "error: " << e.error() << " for arg " << e.argId();
	// //		exit(-1);
	// //	}
	//
	//
	// const bool needDepth = false; //( svoOutputArg.isSet() ? false : true );
	// const sl::zed::ZEDResolution_mode zedResolution = parseResolution( resolutionArg.getValue() );
	// const int whichGpu = -1;
	//
	// sl::zed::Camera *camera = NULL;
	//
	// //LOG_IF( FATAL, calibOutputArg.isSet() && svoOutputArg.isSet() ) << "Calibration data is only generated when using live video, not when recording to SVO.";
	//
	// //LOG(INFO) << "Using live Zed data";
	// camera = new sl::zed::Camera(  zedResolution ); //, 60.0 ); //fpsArg.getValue() );
	//
	// sl::zed::ERRCODE err = sl::zed::LAST_ERRCODE;
	// sl::zed::InitParams initParams;
	// const sl::zed::MODE zedMode = sl::zed::MODE::NONE;  //(needDepth ? sl::zed::MODE::PERFORMANCE : sl::zed::MODE::NONE);
	// initParams.mode = zedMode;
	// const bool verboseInit = true;
	// initParams.verbose = verboseInit;
	// //initParams.disableSelfCalib = true; // disableSelfCalibSwitch.getValue();
	// err = camera->init( initParams );
	//
	// if (err != sl::zed::SUCCESS) {
	// 	LOG(FATAL) << "Unable to init the Zed camera (" << err << "): " << errcode2str(err);
	// 	delete camera;
	// 	exit(-1);
	// }
	//
	// if( svoOutputArg.isSet() ) {
	// 	err = camera->enableRecording( svoOutputArg.getValue() );
	//
	// 	if (err != sl::zed::SUCCESS) {
	// 		LOG(FATAL) << "Error while setting up logging (" << err << "): " << errcode2str(err);
	// 	}
	// }
	//
	// const float fps = 0.0; //= dataSource->fps();
	//
	// //dataSource = new ZedSource( camera, needDepth );
	//
	// //if( calibOutputArg.isSet() ) {
	// //		LOG(INFO) << "Saving calibration to \"" << calibOutputArg.getValue() << "\"";
	// //		calibrationFromZed( camera, calibOutputArg.getValue() );
	// //}
	//
	// int numFrames = 0; //dataSource->numFrames();
	//
	// // Wait for the auto exposure and white balance
	// std::this_thread::sleep_for(std::chrono::seconds(1));
	//
	// const int duration = durationArg.getValue();
	//
	// if( duration > 0 ) {
	// 	LOG(INFO) << "Will log for " << duration << " seconds or press CTRL-C to stop.";
	// } else {
	// 	LOG(INFO) << "Logging now, press CTRL-C to stop.";
	// }
	//
	// std::chrono::steady_clock::time_point start( std::chrono::steady_clock::now() );
	// std::chrono::steady_clock::time_point end( start + std::chrono::seconds( duration ) );
	//
	// int count = 0, miss = 0, displayed = 0;
	// bool logOnce = true;
	// while( keepGoing ) {
	//
	// 	if( count > 0 && (count % 100)==0 ) {
	// 		LOG_IF(INFO, logOnce) << count << " frames";
	// 		logOnce = false;
	// 	} else {
	// 		logOnce = true;
	// 	}
	//
	// 	std::chrono::steady_clock::time_point loopStart( std::chrono::steady_clock::now() );
	// 	if( (duration > 0) && (loopStart > end) ) { keepGoing = false;  break; }
	//
	// 	if( !camera->grab( sl::zed::STANDARD, false, false, false ) ) {
	// 		const bool doDisplayThisFrame = (doDisplay && (count % skip == 0));
	//
	// 		if( svoOutputArg.isSet() ) {
	//
	// 			camera->record();
	//
	// 			if( doDisplayThisFrame ) {
	// 				// According to the docs, this:
	// 				//		Get[s] the current side by side YUV 4:2:2 frame, CPU buffer.
	// 				sl::zed::Mat slRawImage( camera->getCurrentRawRecordedFrame() );
	// 				// Make a copy before enqueueing
	//
	// 				Mat rawCopy;
	// 				sl::zed::slMat2cvMat( slRawImage ).reshape( 2, 0 ).copyTo( rawCopy );
	//
	// 				display.showRawStereoYUV( rawCopy );
	// 				display.waitKey();
	//
	// 				++displayed;
	//
	// 			}
	//
	// 		} else if ( doDisplayThisFrame ) {
	// 				// If you aren't recording, just grab data conventionally
	//
	// 				sl::zed::Mat leftImage( camera->retrieveImage( sl::zed::LEFT ) );
	// 				Mat copy;
	// 				sl::zed::slMat2cvMat( leftImage ).copyTo( copy );
	//
	// 				display.showLeft( copy );
	// 				display.waitKey();
	//
	// 		}
	//
	// 		++count;
	// 		std::this_thread::sleep_for(std::chrono::microseconds(1));
	//
	// 	} else {
	// 		// if grab() fails
	// 		++miss;
	// 		std::this_thread::sleep_for(std::chrono::microseconds(100));
	// 	}
	//
	//
	//
	// 		//			if( dt_us > 0 ) {
	// 		//				std::chrono::steady_clock::time_point sleepTarget( loopStart + std::chrono::microseconds( dt_us ) );
	// 		//				//if( std::chrono::steady_clock::now() < sleepTarget )
	// 		//				std::this_thread::sleep_until( sleepTarget );
	// 		//			}
	//
	// 		if( numFramesArg.isSet() && count >= numFramesArg.getValue() ) keepGoing = false;
	//
	// 	}
	//
	// 	std::chrono::duration<float> dur( std::chrono::steady_clock::now()  - start );
	//
	// 	LOG(INFO) << "Cleaning up...";
	// 	if( camera && svoOutputArg.isSet() ) camera->stopRecording();
	// 	if( camera ) delete camera;
	//
	//
	//
	// 	LOG(INFO) << "Recorded " << count << " frames in " <<   dur.count();
	// 	LOG(INFO) << " Average of " << (float)count / dur.count() << " FPS";
	// 	LOG(INFO) << "   " << miss << " / " << (miss+count) << " misses";
	// 	LOG_IF( INFO, displayed > 0 ) << "   Displayed " << displayed << " frames";
	//
	// 	std::string fileName(svoOutputArg.getValue());
	//
	//
	// 	if( !fileName.empty() ) {
	// 		unsigned int fileSize = fs::file_size( fs::path(fileName));
	// 		float fileSizeMB = float(fileSize) / (1024*1024);
	// 		LOG(INFO) << "Resulting file is " << fileSizeMB << " MB";
	// 		LOG(INFO) << "     " << fileSizeMB/dur.count() << " MB/sec";
	// 		LOG(INFO) << "     " << fileSizeMB/count << " MB/frame";
	//
	//
	// 		if( statisticsOutputArg.isSet() ) {
	// 			ofstream out( statisticsOutputArg.getValue(), ios_base::out | ios_base::ate | ios_base::app );
	// 			if( out.is_open() ) {
	// 				if( statisticsIdArg.isSet() )
	// 				out << statisticsIdArg.getValue() << ',' << resolutionToString( zedResolution ) << "," << fps << "," << (guiSwitch.isSet() ? "display" : "") << "," << count << "," << dur.count() << "," << fileSizeMB << endl;
	// 				else
	// 				out << resolutionToString( zedResolution ) << "," << fps << "," << (guiSwitch.isSet() ? "display" : "") << "," << count << "," << dur.count() << "," << fileSizeMB << endl;
	// 			}
	// 		}
	// 	}
	//

		return 0;
	}
