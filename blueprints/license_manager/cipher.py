# blueprints/documented_endpoints/cipher.py
from flask import Flask, request, jsonify, send_from_directory, abort, send_file
from flask_restx import Namespace, Resource, fields
from http import HTTPStatus
from lic_manager import ENC_ITER, ENC_PASS, LICENSEE_SIGNATURE, P_DATA, License, licenseInfo, getAuthorityIdSecret, issuing, encrypt, decrypt, validate, to_int
from werkzeug.datastructures import FileStorage
import io
import json
from datetime import datetime

ns = Namespace("cipher", "Endpoint for encoding and decoding a file")

upload_parser = ns.parser()
upload_parser.add_argument('file', location='files',
                           type=FileStorage, required=True)
upload_parser.add_argument('enc_pass', location='form',
                           required=True, default=ENC_PASS)
upload_parser.add_argument('enc_iter', location='form',
                           type=int, required=True, default=ENC_ITER)


@ns.route("/encoding", methods=["POST"])
class EncodingClass(Resource):
    @ns.expect(upload_parser)
    def post(self):
        """Endpoint for encoding a file"""
        try:
            args = upload_parser.parse_args()
            file_ = args['file']
            data = file_.read()
            enc_pass = args["enc_pass"]
            enc_iter = args["enc_iter"]

            rawData = P_DATA()
            encData = P_DATA()
            rawData.m_write(data, len(data), True)
            encrypt(rawData, encData, enc_pass, enc_iter)
            return send_file(
                io.BytesIO(encData.m_read(encData.size, False)),
                mimetype='application/octet-stream',
                as_attachment=True,
                download_name=f'{datetime.now().strftime("%Y-%m-%d-%H-%M-%S-%f")}.enc'
            )
        except Exception as em:
            return json.dumps({"error": str(em)}), 500


@ns.route("/decoding", methods=["POST"])
class DecodingClass(Resource):
    @ns.expect(upload_parser)
    def post(self):
        """Endpoint for decoding a file"""
        try:
            args = upload_parser.parse_args()
            file_ = args['file']
            data = file_.read()
            enc_pass = args["enc_pass"]
            enc_iter = args["enc_iter"]

            encData = P_DATA()
            decData = P_DATA()
            encData.m_write(data, len(data), True)
            decrypt(encData, decData, enc_pass, enc_iter)
            return send_file(
                io.BytesIO(decData.m_read(decData.size, False)),
                mimetype='application/octet-stream',
                as_attachment=True,
                download_name=f'{datetime.now().strftime("%Y-%m-%d-%H-%M-%S-%f")}.dec'
            )
        except Exception as em:
            return json.dumps({"error": str(em)}), 500
