#include "license_manager.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;
using namespace P_LIC;

PYBIND11_MODULE(lic_manager, m)
{
    py::class_<P_DATA>(m, "P_DATA")
        .def(py::init<>())
        .def(py::init<const P_DATA &>())
        .def_readonly("size", &P_LIC::P_DATA::size)
        .def_readonly("read", &P_LIC::P_DATA::read)
        .def("clear", &P_DATA::clear)
        // .def("m_write", &P_DATA::m_write, "src"_a, "num"_a, "clear_data"_a = false)
        .def("m_write", &P_DATA::m_write_c, "src"_a, "num"_a, "clear_data"_a = false)
        // .def("m_read", &P_DATA::m_read, "dst"_a, "num"_a, "from_begin"_a = false)
        .def(
            "m_read",
            [](P_DATA &p_data, size_t num, bool from_begin = false) -> py::bytes {
                char *data = new char[num];
                size_t n = p_data.m_read((void *)data, num, from_begin);
                py::bytes bytes(reinterpret_cast<const char *>(data), n);
                delete[] data;
                return bytes;
            },
            "num"_a, "from_begin"_a = false, py::return_value_policy::take_ownership)
        .def("load", py::overload_cast<std::string &, bool>(&P_DATA::load), "filename"_a, "clear_data"_a = false)
        .def("load", py::overload_cast<char *, bool>(&P_DATA::load), "filename"_a, "clear_data"_a = false)
        .def("save", py::overload_cast<std::string &>(&P_DATA::save), "filename"_a)
        .def("save", py::overload_cast<char *>(&P_DATA::save), "filename"_a)
        .def("save_all", py::overload_cast<std::string &>(&P_DATA::save_all), "filename"_a)
        .def("save_all", py::overload_cast<char *>(&P_DATA::save_all), "filename"_a)
        .def("__repr__",
             [](const P_DATA &data) {
                 return std::string("[ ADDRESS ] ") + std::to_string((long long)data.ptr) +
                        std::string("  -  [ SIZE ] ") + std::to_string(data.size - data.read);
             })
        .def("show", &P_DATA::show)
        .def("show_all", &P_DATA::show_all)
        .def("__assign__", &P_DATA::operator=); // expose the assignment operator

    py::class_<licensepp::License>(m, "License")
        .def(py::init<>())
        .def("setLicensee", &licensepp::License::setLicensee)
        .def("setIssuingAuthorityId", &licensepp::License::setIssuingAuthorityId)
        .def("setLicenseeSignature", &licensepp::License::setLicenseeSignature)
        .def("setAuthoritySignature", &licensepp::License::setAuthoritySignature)
        .def("setExpiryDate", &licensepp::License::setExpiryDate)
        .def("setIssueDate", &licensepp::License::setIssueDate)
        .def("setAdditionalPayload", &licensepp::License::setAdditionalPayload)
        .def("licensee", &licensepp::License::licensee)
        .def("issuingAuthorityId", &licensepp::License::issuingAuthorityId)
        .def("licenseeSignature", &licensepp::License::licenseeSignature)
        .def("authoritySignature", &licensepp::License::authoritySignature)
        .def("expiryDate", &licensepp::License::expiryDate)
        .def("issueDate", &licensepp::License::issueDate)
        .def("additionalPayload", &licensepp::License::additionalPayload)
        .def("toString", &licensepp::License::toString)
        .def("raw", &licensepp::License::raw)
        .def("formattedExpiry", &licensepp::License::formattedExpiry)
        .def("load", &licensepp::License::load)
        .def("loadFromFile", &licensepp::License::loadFromFile);

    py::class_<licenseInfo>(m, "licenseInfo")
        .def(py::init<>())
        .def(py::init<std::string &, std::string &, std::string &, std::string &, std::string &, unsigned int>())
        .def_readwrite("licenseeSignature", &licenseInfo::licenseeSignature)
        .def_readwrite("licensee", &licenseInfo::licensee)
        .def_readwrite("secret", &licenseInfo::secret)
        .def_readwrite("authorityId", &licenseInfo::authorityId)
        .def_readwrite("additionalPayload", &licenseInfo::additionalPayload)
        .def_readwrite("period", &licenseInfo::period);

    py::enum_<licensepp::VALIDATE_ERROR_ENUM>(m, "VALIDATE_ERROR_ENUM")
        .value("LICENSE_IS_VALID", licensepp::VALIDATE_ERROR_ENUM::LICENSE_IS_VALID)
        .value("FAILED_VERIFY_LICENSING_AUTHORITY", licensepp::VALIDATE_ERROR_ENUM::FAILED_VERIFY_LICENSING_AUTHORITY)
        .value("LICENSE_WAS_EXPIRED", licensepp::VALIDATE_ERROR_ENUM::LICENSE_WAS_EXPIRED)
        .value("SHOULD_VERIFY_SIGNATURE", licensepp::VALIDATE_ERROR_ENUM::SHOULD_VERIFY_SIGNATURE)
        .value("FAILED_AES_ENCRYP", licensepp::VALIDATE_ERROR_ENUM::FAILED_AES_ENCRYPT)
        .export_values();
    m.def("to_int", [](licensepp::VALIDATE_ERROR_ENUM e) {
        return static_cast<std::underlying_type_t<licensepp::VALIDATE_ERROR_ENUM>>(e);
    });

    py::class_<licensepp::VALIDATE_ERROR>(m, "VALIDATE_ERROR")
        .def_readwrite("error_code", &licensepp::VALIDATE_ERROR::error_code)
        .def_readwrite("message", &licensepp::VALIDATE_ERROR::message);

    // Expose the functions
    m.def("getAuthorityIdSecret", &getAuthorityIdSecret, "Id"_a, "lInfo"_a);
    m.def("encrypt", py::overload_cast<FILE *, FILE *, const char *, int>(&encrypt), "ifp"_a, "ofp"_a,
          "enc_pass"_a = ENC_PASS, "enc_iter"_a = ENC_ITER);
    m.def("encrypt", py::overload_cast<P_DATA &, P_DATA &, const char *, int>(&encrypt), "idata"_a, "odata"_a,
          "enc_pass"_a = ENC_PASS, "enc_iter"_a = ENC_ITER);
    m.def("decrypt", py::overload_cast<FILE *, FILE *, const char *, int>(&decrypt), "ifp"_a, "ofp"_a,
          "enc_pass"_a = ENC_PASS, "enc_iter"_a = ENC_ITER);
    m.def("decrypt", py::overload_cast<P_DATA &, P_DATA &, const char *, int>(&decrypt), "idata"_a, "odata"_a,
          "enc_pass"_a = ENC_PASS, "enc_iter"_a = ENC_ITER);
    m.def("showLicenseInfo", &showLicenseInfo, "license"_a);
    m.def("issuing", py::overload_cast<licenseInfo &, licensepp::License &>(&issuing), "lInfo"_a, "license"_a);
    m.def("issuing", py::overload_cast<licenseInfo &, std::string>(&issuing), "lInfo"_a, "licPath"_a = "EMoi.lic");
    m.def("issuing", py::overload_cast<licenseInfo &, P_DATA &>(&issuing), "lInfo"_a, "odata"_a);
    m.def("validateFromFile", &validateFromFile, "license_file"_a, "license"_a);
    m.def("validate", py::overload_cast<std::string, licensepp::License &>(&validate), "license_string"_a, "license"_a);
    m.def("validate", py::overload_cast<P_DATA &, licensepp::License &>(&validate), "idata"_a, "license"_a);

    m.attr("ENC_ITER") = ENC_ITER;
    m.attr("ENC_PASS") = ENC_PASS;
    m.attr("LICENSEE_SIGNATURE") = LICENSEE_SIGNATURE;
}
