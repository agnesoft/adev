#ifndef __clang__
export module abuild.cache:visibility;
#endif

namespace abuild
{
//! The `Visibility` determines if the given
//! declartion is visible outside of the unit
//! where it was declared. I.e. when an entity is
//! declared `export` it will have visibility
//! `Public`. Otherwise it will have visibility
//! `Private`.
export enum class Visibility
{
    //! The entity is visible outside of the
    //! current unit.
    Public,

    //! The entity is internal to this unit.
    Private
};
}