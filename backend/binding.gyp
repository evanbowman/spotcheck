{
    'targets': [
	{
	    'target_name': 'backendlib',
	    'conditions': [
		['OS=="mac"', {
		    'link_settings': {
			'libraries': [
                            '-L/usr/local/lib',
			    '-lopencv_highgui',
                            '-lopencv_imgproc',
                            '-lopencv_core'
                        ]
		    }
		}],
		['OS=="win"', {
		    'include_dirs': [
			'C:\\opencv\\build\\include'
		    ],
                    # link_setting for windows is out of date, !!add yaml-cpp to libraries or revert to previous commit
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
		'src/preview_normalized.cpp',
		'src/results.cpp',
		'src/analysis.cpp',

	    ]
	}
    ]
}
