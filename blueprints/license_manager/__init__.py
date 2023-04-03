# blueprints/documented_endpoints/__init__.py
from flask import Blueprint
from flask_restx import Api

from .license import ns as license_ns
from .cipher import ns as cipher_ns

blueprint = Blueprint("documented_api", __name__, url_prefix="/")

api_extension = Api(
    blueprint,
    title="License Manager",
    version="1.0",
    description="License++ is software licensing library that provides an abstract way to secure your software usage.",
    doc="/",
)

api_extension.add_namespace(license_ns)
api_extension.add_namespace(cipher_ns)
