{
  "targets": [
    {
      "target_name": "audio",
      "sources": [ "audio.cc" ],
      "cflags" : [ "-lole32", "-loleaut32"],
      "include_dirs" : [
          "<!(node -e \"require('nan')\")"
      ]
    }
  ],
  
}
