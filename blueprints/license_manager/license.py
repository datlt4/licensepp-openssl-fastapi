# blueprints/documented_endpoints/license.py
from flask import send_file
from flask_restx import Namespace, Resource
from lic_manager import ENC_ITER, ENC_PASS, LICENSEE_SIGNATURE, P_DATA, License, licenseInfo, getAuthorityIdSecret, issuing, encrypt, decrypt, validate, to_int
from werkzeug.datastructures import FileStorage
import io
import json
from uuid import uuid4

ns = Namespace("license", "Endpoint for issuing and validating a license")

lic_info_parser = ns.parser()
lic_info_parser.add_argument("serial", location="form", required=True)
lic_info_parser.add_argument(
    "period", location="form", required=False, type=int, default=86400)
lic_info_parser.add_argument(
    "licensee", location="form", required=False, default="EMoi_ltd")
lic_info_parser.add_argument(
    'enc_pass', location='form', required=False, default=ENC_PASS)
lic_info_parser.add_argument(
    'enc_iter', location='form', type=int, required=False, default=ENC_ITER)


@ns.route("/issuing")
class IssuingClass(Resource):
    @ns.expect(lic_info_parser)
    @ns.response(200, "License file generated successfully")
    @ns.response(500, "Internal Server error")
    def post(self):
        """Endpoint for issuing a license"""
        try:
            args = lic_info_parser.parse_args()
            additionalPayload = args["serial"]
            period = args['period']
            Id = "c1"
            licensee = args["licensee"]
            enc_pass = args["enc_pass"]
            enc_iter = args["enc_iter"]

            if additionalPayload is None:
                return json.dumps({"message": "`serial` field not found"}), 400

            lInfo = licenseInfo(LICENSEE_SIGNATURE, licensee,
                                f"{Id}-secret-passphrase", Id, additionalPayload,  period)
            getAuthorityIdSecret(Id, lInfo)

            licData = P_DATA()
            encData = P_DATA()
            issuing(lInfo, licData)
            encrypt(licData, encData, enc_pass, enc_iter)
            return send_file(
                io.BytesIO(encData.m_read(encData.size, False)),
                mimetype='application/octet-stream',
                as_attachment=True,
                download_name=f'{uuid4().hex}.lic'
            )
        except Exception as em:
            return (json.dumps({"message": em})), 500


upload_parser = ns.parser()
upload_parser.add_argument('file', location='files',
                           type=FileStorage, required=True)
upload_parser.add_argument('enc_pass', location='form',
                           required=True, default=ENC_PASS)
upload_parser.add_argument('enc_iter', location='form',
                           type=int, required=True, default=ENC_ITER)


@ns.route("/validating", methods=["POST"])
class ValidatingClass(Resource):
    @ns.expect(upload_parser)
    def post(self):
        """Endpoint for validating a license"""
        try:
            args = upload_parser.parse_args()
            file_ = args['file']
            data = file_.read()
            enc_pass = args["enc_pass"]
            enc_iter = args["enc_iter"]

            encData = P_DATA()
            decData = P_DATA()
            license = License()
            encData.m_write(data, len(data), True)
            decrypt(encData, decData, enc_pass, enc_iter)
            val_err = validate(decData, license)
            if to_int(val_err.error_code):
                return json.dumps({"invalid": val_err.message}), 422
            else:
                return json.loads(license.raw()), 200
        except Exception as em:
            return json.dumps({"error": str(em)}), 500
