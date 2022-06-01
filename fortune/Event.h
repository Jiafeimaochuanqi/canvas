/* FortuneAlgorithm
 * Copyright (C) 2018 Pierre Vigier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// My includes
#include "Vector2.h"
#include "VoronoiDiagram.h"

class Arc;

class Event
{
public:
    enum class Type{SITE, CIRCLE};

    
    // Circle event
    Event(double y, Vector2 point, Arc* arc): type(Type::CIRCLE), y(y), index(-1), point(point), arc(arc)
    {


    }
	// Site event
    Event(VoronoiDiagram::Site* site): type(Type::SITE), y(site->point.y), index(-1), site(site)
    {

    }

    const Type type;
    double y;
    int index;
    // Site event
    VoronoiDiagram::Site* site;
    // Circle event
    Vector2 point;
    Arc* arc;

};

bool operator<(const Event& lhs, const Event& rhs);
std::ostream& operator<<(std::ostream& os, const Event& event);

