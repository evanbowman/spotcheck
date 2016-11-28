{
    'targets': [
	{
	    'target_name': 'backendlib',
	    'conditions': [
		['OS=="mac"', {
		    'link_settings': {
			'libraries': ['-L/usr/local/lib', '-lopencv_highgui', '-lopencv_imgproc']
		    }
		}],
		['OS=="win"', {
		    'include_dirs': [
			'C:\\opencv\\build\\include'
		    ],
		    'link_settings': {
			'libraries': ['C:\\opencv\\build\\x64\\vc12\\staticlib\\opencv_imgproc2413.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\opencv_highgui2413.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\opencv_core2413.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\libjpeg.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\libpng.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\libtiff.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\libjasper.lib',
				      'C:\\opencv\\build\\x64\\vc12\\staticlib\\IlmImf.lib']
		    }
		}]
	    ],
	    'sources': [
		'src/addon.cpp',
		'src/backend.cpp',
		'src/analyze.cpp',
		'src/test_thresh.cpp',
		'src/spot.cpp',
	    ]
	}
    ]
}
