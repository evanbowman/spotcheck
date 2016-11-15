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
          'link_settings': {
	    'libraries': []
	  }
        }]
      ],
      'sources': [
        'src/addon.cpp',
        'src/backend.cpp',
        'src/circ_score.cpp',
	'src/parse_gal.cpp',
      ]
    }
  ]
}
