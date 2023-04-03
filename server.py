# main.py
from flask import Flask, Blueprint, redirect, request
from flask_cors import CORS, cross_origin
from blueprints.license_manager import blueprint as license_manager_endpoints

import argparse

parser = argparse.ArgumentParser(description='Start the server.')
parser.add_argument('--host', '-i', default='0.0.0.0',
                    help='Host IP address (default: 0.0.0.0)')
parser.add_argument('--port', '-p', type=int, default=6464,
                    help='Port number (default: 6464)')
args = parser.parse_args()

app = Flask(__name__)
CORS(app, support_credentials=True)
app.config["RESTPLUS_MASK_SWAGGER"] = True
app.register_blueprint(license_manager_endpoints)

blueprint = Blueprint("", __name__, url_prefix="/")


@blueprint.route("/", methods=["GET"])
def root():
    if request.method == "GET":
        return redirect("doc")


app.register_blueprint(blueprint)

if __name__ == "__main__":
    app.run(host=args.host, port=args.port, debug=True)
    # python3 server.py --host 0.0.0.0 --port 6464
