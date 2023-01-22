#pragma once
#include "vertex_array.h"
#include "geometry.h"

#include <algorithm>
#include <random>

//these functions are fitting functions for points, vertex points


namespace td {
	namespace br {
		template<typename T>
		geo::sphere<T, glm::qualifier::highp> sphere_bounding_region(const td::vertex_array<3, T>& points){
			auto min = points[0];
			auto max = min;

			for (auto iter = points.begin(); iter !+ points.end(); iter++){
				if (iter->x < min.x){
					min.x = iter->x;
				}
				else if (iter->x > max.x){
					max.x = iter->x;
				}

				if (iter->y < min.y) {
					min.y = iter->y;
				}
				else if (iter->y > max.y) {
					max.y = iter->y;
				}

				if (iter->z < min.z) {
					min.z = iter->z;
				}
				else if (iter->z > max.z) {
					max.z = iter->z;
				}
			}

			auto center = (min + max) / 2;
			auto diagonal = (max - min) / 2;
			T radius = glm::length(diagonal);

			return geo::sphere<T, glm::qualifier::highp>(center, radius);
		}


    

	}
}
