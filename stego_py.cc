#include "stego_storage.h"

#include <boost/python.hpp>


void (stego_disk::StegoStorage::*Configure1)() const = &stego_disk::StegoStorage::Configure;
void (stego_disk::StegoStorage::*Configure2)(const std::string&) const = &stego_disk::StegoStorage::Configure;
void (stego_disk::StegoStorage::*Configure3)(const stego_disk::EncoderFactory::EncoderType encoder,
                                             const stego_disk::PermutationFactory::PermutationType global_perm,
                                             const stego_disk::PermutationFactory::PermutationType local_perm) const = &stego_disk::StegoStorage::Configure;

void Write(stego_disk::StegoStorage& self, boost::python::object buffer) {

  std::size_t size =  boost::python::len(buffer);

  if(size > self.GetSize())
    throw std::runtime_error("Stego has not enough space");

  PyObject *py_ba = buffer.ptr();

  if (PyByteArray_Check(py_ba)) {
    self.Write(static_cast<void*>(PyByteArray_AsString(py_ba)), 0, size);
  } else if(PyBytes_Check(py_ba)) {
    self.Write(static_cast<void*>(PyBytes_AsString(py_ba)), 0, size);
  } else {
    throw std::runtime_error("write() type of input object is not a bytearray or a bytes!");
  }
}

boost::python::object Read(stego_disk::StegoStorage& self, std::size_t size){

  if(size > self.GetSize())
    throw std::runtime_error("Stego has not enough space");

  unsigned char* buff = static_cast<unsigned char*>(malloc(size * sizeof(unsigned char)));

  self.Read(buff, 0, size);

  boost::python::object py_obj = boost::python::object(boost::python::handle<PyObject>(
                                                         PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(buff), size)));

  free(buff);

  return py_obj;
}

BOOST_PYTHON_MODULE(stego_py) {


  boost::python::class_<stego_disk::StegoStorage, boost::noncopyable>("StegoStorage")
      .def("open", &stego_disk::StegoStorage::Open)
      .def("load", &stego_disk::StegoStorage::Load)
      .def("save", &stego_disk::StegoStorage::Save)
      .def("read", Read)
      .def("write", Write)
      .def("configure", Configure1)
      .def("configure", Configure2)
      .def("configure", Configure3)
      .def("get_size", &stego_disk::StegoStorage::GetSize)
      ;

  boost::python::enum_<stego_disk::EncoderFactory::EncoderType>("encoder")
      .value("lsb", stego_disk::EncoderFactory::EncoderType::LSB)
      .value("hamming", stego_disk::EncoderFactory::EncoderType::HAMMING)
      ;

  boost::python::enum_<stego_disk::PermutationFactory::PermutationType>("permutation")
      .value("identity", stego_disk::PermutationFactory::PermutationType::IDENTITY)
      .value("affine", stego_disk::PermutationFactory::PermutationType::AFFINE)
      .value("affine64", stego_disk::PermutationFactory::PermutationType::AFFINE64)
      .value("feistel_num", stego_disk::PermutationFactory::PermutationType::FEISTEL_NUM)
      .value("feistel_mix", stego_disk::PermutationFactory::PermutationType::FEISTEL_MIX)
      ;
}
