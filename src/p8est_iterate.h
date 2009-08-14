/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2009 Carsten Burstedde, Lucas Wilcox,
                     Toby Isaac.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef P8EST_ITERATE_H
#define P8EST_ITERATE_H

#include <p8est.h>

SC_EXTERN_C_BEGIN;

/** The information that is available to the user-defined p8est_iter_volume_t
 * callback function.
 *
 * \a treeid gives the index in \a p4est->trees of the tree to which
 *    \a quad belongs.
 * \a quadid gives the index of \a quad within \a tree's quadrants array.
 */
typedef struct p8est_iter_volume_info
{
  p8est_t            *p4est;
  sc_array_t         *ghost_layer;
  p8est_quadrant_t   *quad;
  size_t              quadid;
  p4est_topidx_t      treeid;
}
p8est_iter_volume_info_t;

/** The prototype for a function that p8est_iterate will execute at every
 * quadrant local to the current process.
 */
typedef void        (*p8est_iter_volume_t) (p8est_iter_volume_info_t * info,
                                            void *user_data);

/* Information about one side of a face in the forest.  If a \a quad is local,
 * then its \a quadid indexes the tree's quadrant array; otherwise, it indexes
 * the ghosts array. If the face is hanging, then the quadrants are listed in
 * z-order. */
typedef struct p8est_iter_face_side
{
  p4est_topidx_t      treeid;
  int                 face;
  bool                is_hanging;
  union p8est_iter_face_side_data
  {
    struct
    {
      p8est_quadrant_t   *quad;
      bool                is_local;
      p4est_locidx_t      quadid;
    }
    full;
    struct
    {
      p8est_quadrant_t   *quad[4];
      bool                is_local[4];
      p4est_locidx_t      quadid[4];
    }
    hanging;
  }
  is;
}
p8est_iter_face_side_t;

/** The information about both sides of a face in the forest.  The orientation
 * is 0 if the face is within one tree; otherwise, it is the same as the
 * orientation value between the two trees given in the connectivity.  If the
 * face is on the outside of the forest, then sides will have only one element.
 */
typedef struct p8est_iter_face_info
{
  p4est_t            *p4est;
  sc_array_t         *ghost_layer;
  int                 orientation;
  sc_array_t          sides;    /* p8est_iter_face_side_t */
}
p8est_iter_face_info_t;

/** The prototype for a function that p8est_iterate will execute wherever two
 * quadrants share a face: the face can be a 2:1 hanging face, it does not have
 * to be conformal.
 *
 * Note: the forest must be face balanced for p8est_iterate to execute a
 * callback function on faces.
 */
typedef void        (*p8est_iter_face_t) (p8est_iter_face_info_t * info,
                                          void *user_data);

typedef struct p8est_iter_edge_side
{
  p4est_topidx_t      treeid;
  int                 edge;
  bool                is_hanging;
  int                 orientation;
  union p8est_iter_edge_side_data
  {
    struct
    {
      p8est_quadrant_t   *quad;
      bool                is_local;
      p4est_locidx_t      quadid;
    }
    full;
    struct
    {
      p8est_quadrant_t   *quad[2];
      bool                is_local[2];
      p4est_locidx_t      quadid[2];
    }
    hanging;
  }
  is;
}
p8est_iter_edge_side_t;

typedef struct p8est_iter_edge_info2
{
  p4est_t            *p4est;
  sc_array_t         *ghost_layer;
  sc_array_t          sides;    /* p8est_iter_edge_side_t */
}
p8est_iter_edge_info2_t;

/** The information that is available to the user defined p8est_iter_edge_t
 * callback function about the quadrants surrounding an edge.
 * There may be a variable number of quadrants.  If \a is_hanging is true, then
 * some of the quadrants around the edge are half the size of the the others.
 * However, if this is the case, then they all meet at a corner, and
 * \a common_corners gives the corner id for the shared corner from each
 * quadrant.  As above, a\ quadids may be positive or negative.
 */
typedef struct p8est_iter_edge_info
{
  p8est_t            *p4est;
  sc_array_t         *ghost_layer;
  sc_array_t         *quads;          /** elements are (p8est_quadrant_t *) */
  sc_array_t         *quadids;        /** elements are (ssize_t)            */
  sc_array_t         *treeids;        /** elements are (p4est_locidx_t)     */
  sc_array_t         *edges;          /** elements are (int)                */
  sc_array_t         *common_corners; /** elements are (int)                */
  bool                is_hanging;
}
p8est_iter_edge_info_t;

/** The prototype for a function that p8est_iterate will execute wherever
 * quadrants meet at a conformal edge i.e. the callback will not execute on
 * an edge the sits on a hanging face.
 *
 * Note: the forest must be edge balanced for p8est_iterate to execute a
 * callback function on edges.
 */
typedef void        (*p8est_iter_edge_t) (p8est_iter_edge_info_t * info,
                                          void *user_data);

typedef struct p8est_iter_corner_side
{
  p4est_topidx_t      treeid;
  int                 corner;
  p8est_quadrant_t   *quad;
  bool                is_local;
  p4est_locidx_t      quadid;
}
p8est_iter_corner_side_t;

typedef struct p8est_iter_corner_info2
{
  p4est_t            *p4est;
  sc_array_t         *ghost_layer;
  sc_array_t          sides;    /* p8est_iter_corner_side_t */
}
p8est_iter_corner_info2_t;

/** The information that is available to the user defined p8est_iter_corner_t
 * callback function about the quadrants surrounding a corner.
 * There may be a variable number of quadrants. \a corners gives the **z-order**
 * corner that touches the other shared corners, all other information is as
 * above.
 */
typedef struct p8est_iter_corner_info
{
  p8est_t            *p4est;
  sc_array_t         *ghost_layer;
  sc_array_t         *quads;      /** elements are (p8est_quadrant_t *) */
  sc_array_t         *quadids;    /** elements are (ssize_t)            */
  sc_array_t         *treeids;    /** elements are (p4est_locidx_t)     */
  sc_array_t         *corners;    /** elements are (int)                */
}
p8est_iter_corner_info_t;

/** The prototype for a function that p8est_iterate will execute wherever
 * quadrants meet at a conformal corner i.e. the callback will not execute on
 * a corner that sits on a hanging face or edge.
 *
 * Note: the forest does not need to be corner balanced for p8est_iterate to
 * execute a callback function at corners, only face and edge balance.
 * However, the ghost_layer must be created with the P4EST_BALANCE_FULL option.
 */
typedef void        (*p8est_iter_corner_t) (p8est_iter_corner_info_t * info,
                                            void *user_data);

/** p8est_iterate executes the user-supplied callback functions at every
 * volume, face, edge and corner in the local forest. The \a user_data pointer
 * is not touched by p4est_iterate, but is passed to each of the callbacks.
 * The callback functions are interspersed with each other, i.e. some face
 * callbacks will occur between volume callbacks, and some edge callbacks
 * will occur between face callbacks, etc.:
 *
 * 1) volume callbacks occur in the sorted Morton-index order.
 * 2) a face callback is not executed until after the volume callbacks have
 *    been executed for the quadrants that share it.
 * 3) an edge callback is not executed until the face callbacks have been
 *    executed for all faces that touch the edge.
 * 4) a corner callback is not executed until the edge callbacks have been
 *    executed for all edges that touch the corner.
 * 5) it is not always the case that every face callback for a given quadrant
 *    is executed before any of the edge or corner callbacks, and it is not
 *    always the case that every edge callback for a given quadrant is executed
 *    before any of the corner callbacks.
 * 6) callbacks are not executed at faces, edges or corners that only involve
 *    ghost quadrants, i.e. that are not adjacent in the local section of the
 *    forest.
 */
void                p8est_iterate (p8est_t * p4est, sc_array_t * ghost_layer,
                                   void *user_data,
                                   p8est_iter_volume_t iter_volume,
                                   p8est_iter_face_t iter_face,
                                   p8est_iter_edge_t iter_edge,
                                   p8est_iter_corner_t iter_corner);

SC_EXTERN_C_END;

#endif /* !P8EST_ITERATE_H */