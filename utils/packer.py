import os
import ctypes

class PakerDecoder:
    def __init__(self, lib='build/libpaker_py.so'):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        library_path = os.path.join(current_dir, lib)
        
        self.lib = ctypes.CDLL(library_path)
        self._setup_prototypes()

    def _setup_prototypes(self):
        self.lib.DecodeSuperframe.argtypes = [
            ctypes.POINTER(ctypes.c_char_p),
            ctypes.c_uint8,
            ctypes.POINTER(ctypes.c_uint8),
            ctypes.c_uint16
        ]

    def decode_superframe(self, frames):
        out_buffer = bytearray(256)
        frame_count = len(frames)
        frames_c_array = (ctypes.c_char_p * frame_count)(*frames)
        out_buffer_ptr = (ctypes.c_uint8 * len(out_buffer)).from_buffer(out_buffer)
        self.lib.DecodeSuperframe(frames_c_array, frame_count, out_buffer_ptr, len(out_buffer))
        return out_buffer

# decoder = PakerDecoder()
# frames = ["frame1", "frame2", ...]
# buffer = bytearray(256)
# result = decoder.decode_superframe(frames, buffer)
