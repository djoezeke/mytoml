/**
 * @file mytoml.cppm
 * @brief File containing the module declaration for mytoml.
 */

module;

#include <mytoml/mytoml.hpp>

export module mytoml;

/**
 * @namespace mytoml
 * @brief The mytoml namespace mytoml::
 */
export namespace mytoml {

/**
 * @namespace literals
 * @brief The mytoml namespace mytoml::literals::
 */
inline namespace literals {

using ::mytoml::literals::operator""_toml;

}  // namespace literals

using ::mytoml::toml;

}  // namespace mytoml
