# audio_normalizer
drop in solution for sonarr / radarr to normalize tv and movie audio.

# Requirements

- fdkaac
- faad
- ffmpeg
- mkvtoolnix
- DynamicNormalizeAudioCLI (mono)
- eac3to

# Steps

1. Figure out container format
2. Figure out contents
3. If video needs re-encoding, do it now, threaded
4. If audio is matched language
   1. Extract track
   2. If multi channel down mix to stereo
   3. Normalize audio
   4. Encode as AAC
5. If forced subtitle is matched language then extract
6. Mux video, audio and subtitle into MP4 container (best compat)
7. Move original file out 
8. Move new file in
