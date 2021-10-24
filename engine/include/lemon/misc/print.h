#pragma once

template<typename T, size_t N, glm::qualifier Q>
std::ostream&
operator<<(std::ostream& os, const glm::vec<N, T, Q>& vec)
{
    os << "(";

    for (auto i = 0; i < vec.length(); i++) {
        if (i != 0) {
            os << ", ";
        }

        os << vec[i];
    }

    os << ")";

    return os;
}
