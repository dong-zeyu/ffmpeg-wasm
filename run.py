from RangeHTTPServer import RangeRequestHandler
from http import server # Python 3

class CORSHTTPRequestHandler(RangeRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        super().end_headers()

if __name__ == '__main__':
    server.test(HandlerClass=CORSHTTPRequestHandler)
