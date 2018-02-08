//
// Created by tripack on 17-3-26.
//

#ifndef VE490_UTIL_FINDER_H
#define VE490_UTIL_FINDER_H

#include <string>

namespace ECTL::Utils {

    class File {
        std::string base;
        std::string ext;

    public:
        File(const std::string &base, const std::string &ext);
        File operator[](const std::string &name) const;
        File operator[](int i) const;

        operator std::string() const;
        std::string toString() const;
    };

    class FileClass {
        std::string ext;
    public:
        FileClass(const std::string &ext);
        File operator()(const std::string &name) const;

        std::string toString() const;
    };

    class Path {
        std::string path;
    public:
        Path(const std::string &p);
        std::string toString() const;

        std::string operator/(const std::string &f) const;

        Path sub(const std::string &f) const;
    };

    static const Path ProjectBase(PROJECT_SOURCE_DIR);

    static const FileClass fBlif("blif");
    static const FileClass fCpp("cpp");
    static const FileClass fSo("so");
}

#endif //VE490_RESOURCE_H
