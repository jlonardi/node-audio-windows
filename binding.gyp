{
  "targets": [
    {
      "target_name": "volume_controller",
      "sources": [ "src/volume_controller.cc" ],
      "libraries": [ "ole32.lib"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      'msvs_settings' : {
        'VCCLCompilerTool' : {
          'AdditionalOptions' : ['/EHsc']
        },
      }
    }
  ]
}