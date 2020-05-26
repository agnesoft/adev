#ifndef ACORE_UNDOSTACK_HPP
#define ACORE_UNDOSTACK_HPP

#include <memory>
#include <variant>

namespace acore
{
//! The UndoStack is an implementation o the command
//! pattern.
//!
//! The command types must be passed as template
//! arguments when creating the stack. It can take
//! any \c Command if it satisfies the following
//! criteria:
//!
//! - Is move constructible and move assignable.
//! - Command::redo() is a public method that executes the command
//! - Command::undo() is a public method that undoes the changes made by the command
//!
//! Individual commands are pushed onto the stack
//! with push() that takes r-value of the command
//! as the argument. The command is executed immediately
//! after it is pushed onto the stack.
//!
//! The commands can be undone with undo() and redone
//! again with redo(). Convenience methods undoAll()
//! and redoAll() are provided so that you can undo
//! everything in the stack or redo everything that
//! was undone so far.
//!
//! \note When the index() does not point to the
//! last command in the stack (i.e. some commands
//! were already undone) and a new \c Command is
//! pushed onto the stack then the previously undone
//! commands are discarded and the new command becomes
//! new top of the stack.
//!
//! Example:
//! \snippet UndoStackTest.cpp [Usage]
//!
//! The UndoStack is not copyable but is movable.
//! There is no way to get to the previously pushed
//! commands in order to preserve integrity of the
//! stack and the command-managed system.
template<typename... Commands>
class UndoStack
{
public:
    //! Defaulted constructor.
    UndoStack() = default;

    //! This constructor is deleted.
    UndoStack(const UndoStack &other) = delete;

    //! Defaulted move constructs the undo stack
    //! from \a other. The \a other must not be
    //! used afterwards.
    constexpr UndoStack(UndoStack &&other) noexcept = default;

    //! Defaulted destructor.
    ~UndoStack() = default;

    //! Removes all commands from the stack. It does
    //! neither undo() or redo() on the commands
    //! preserving the current state of the system.
    constexpr auto clear() -> void
    {
        mCommands.clear();
        mIndex = INVALID_INDEX;
    }

    //! Returns \c number of commands in the stack
    //! regardless of the current index().
    [[nodiscard]] constexpr auto count() const noexcept -> size_type
    {
        return static_cast<size_type>(mCommands.size());
    }

    //! Returns the <tt>current index</tt> of the
    //! stack. The command under the index will be
    //! undone when undo() is called. The \c next
    //! command on the stack will be redone when
    //! redo() is called.
    [[nodiscard]] constexpr auto index() const noexcept -> size_type
    {
        return mIndex;
    }

    //! Returns \c true if the index() points to the
    //! last command in the stack or if the stack
    //! is empty.
    [[nodiscard]] constexpr auto isClean() const noexcept -> bool
    {
        return static_cast<size_type>(mCommands.size()) == (mIndex + 1);
    }

    //! Pushes the \a command onto the stack and
    //! advances the internal stack index. Calls
    //! <tt>Command::redo()</tt> immediately. The
    //! \a command is moved from and the argument must not
    //! be used afterwards.
    //!
    //! When the index() does not point to the last
    //! command in the stack (i.e. some commands
    //! were undone) and a new \c Command is pushed
    //! onto the stack then the previously undone
    //! commands are discarded and the \a command
    //! becomes the new top of the stack.
    template<typename T>
    constexpr auto push(T &&command) -> void
    {
        mCommands.resize(static_cast<size_t>(mIndex + 1));
        mCommands.emplace_back(std::forward<T>(command));
        doRedo();
    }

    //! Advances the index() to the next command and
    //! calls its \c Command::redo() method. If the
    //! stack is in isClean() state this method
    //! does nothing.
    constexpr auto redo() -> void
    {
        if (!isClean())
        {
            doRedo();
        }
    }

    //! Calls redo() until the index() points to
    //! the last command in the stack. If the
    //! stack is in isClean() state this method
    //! does nothing.
    constexpr auto redoAll() -> void
    {
        while (!isClean())
        {
            doRedo();
        }
    }

    //! Calls \c undo() method on the command under
    //! current index() and decrements the index to
    //! the previous command. Does nothing if the
    //! stack is empty or index() points before the
    //! last command in the stack.
    constexpr auto undo() -> void
    {
        if (mIndex > INVALID_INDEX)
        {
            doUndo();
        }
    }

    //! Calls undo() until the index() points before
    //! the last command in the stack. Does nothing
    //! if the stack is empty or index() is already
    //! pointing before the last command.
    constexpr auto undoAll() -> void
    {
        while (mIndex > INVALID_INDEX)
        {
            doUndo();
        }
    }

    //! The copy assignment operator is deleted.
    auto operator=(const UndoStack &other) -> UndoStack & = delete;

    //! Defaulted move assignment operator assigns
    //! the undo stack from \a other. The \a other
    //! must not be used afterwards.
    constexpr auto operator=(UndoStack &&other) noexcept -> UndoStack & = default;

private:
    auto doRedo() -> void
    {
        auto redoHandler = [](auto &&command) {
            using T = std::decay_t<decltype(command)>;

            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                command.redo();
            }
        };

        ++mIndex;
        std::visit(redoHandler, mCommands[mIndex]);
    }

    auto doUndo() -> void
    {
        auto undoHandler = [](auto &&command) {
            using T = std::decay_t<decltype(command)>;

            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                command.undo();
            }
        };

        std::visit(undoHandler, mCommands[mIndex]);
        --mIndex;
    }

    std::vector<std::variant<std::monostate, Commands...>> mCommands;
    size_type mIndex = INVALID_INDEX;
};
}

#endif
