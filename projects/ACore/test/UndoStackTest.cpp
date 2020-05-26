#include "pch.hpp"

#include <Catch2/catch.hpp>

namespace undostacktest
{
struct CommandCounter
{
    int redo = 0;
    int undo = 0;
};

class Command
{
public:
    explicit Command(CommandCounter *counter) :
        mCounter(counter)
    {
    }

    auto redo() -> void
    {
        ++mCounter->redo;
    }
    auto undo() -> void
    {
        ++mCounter->undo;
    }

private:
    CommandCounter *mCounter = nullptr;
};

class OtherCommand : public Command
{
public:
    using Command::Command;
};

TEST_CASE("[acore::UndoStack]")
{
    REQUIRE(std::is_default_constructible_v<acore::UndoStack<Command, OtherCommand>>);
    REQUIRE_FALSE(std::is_copy_constructible_v<acore::UndoStack<Command, OtherCommand>>);
    REQUIRE(std::is_nothrow_move_constructible_v<acore::UndoStack<Command, OtherCommand>>);
    REQUIRE_FALSE(std::is_copy_assignable_v<acore::UndoStack<Command, OtherCommand>>);
    REQUIRE(std::is_nothrow_move_assignable_v<acore::UndoStack<Command, OtherCommand>>);
    REQUIRE(std::is_nothrow_destructible_v<acore::UndoStack<Command, OtherCommand>>);
}

TEST_CASE("UndoStack() [acore::UndoStack]")
{
    REQUIRE_NOTHROW(acore::UndoStack<Command, OtherCommand>{});
}

TEST_CASE("UndoStack(UndoStack &&other) noexcept [acore:Q:UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    REQUIRE(noexcept(acore::UndoStack<Command, OtherCommand>{std::move(undoStack)})); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    acore::UndoStack<Command, OtherCommand> other{std::move(undoStack)}; //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    other.undo();
    REQUIRE(counter.redo == 2);
    REQUIRE(counter.undo == 1);
}

TEST_CASE("clear() -> void [acore::UndoStack]")
{
    SECTION("[empty]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        undoStack.clear();
        REQUIRE(undoStack.count() == 0);
        REQUIRE(undoStack.index() == acore::INVALID_INDEX);
        REQUIRE(undoStack.isClean());
    }

    SECTION("[data]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        undoStack.push(OtherCommand{&counter});

        SECTION("[clean]")
        {
            undoStack.clear();
            REQUIRE(counter.redo == 2);
            REQUIRE(counter.undo == 0);
            REQUIRE(undoStack.count() == 0);
            REQUIRE(undoStack.index() == acore::INVALID_INDEX);
            REQUIRE(undoStack.isClean());
        }

        SECTION("[undone]")
        {
            undoStack.undo();
            undoStack.clear();
            REQUIRE(counter.redo == 2);
            REQUIRE(counter.undo == 1);
            REQUIRE(undoStack.count() == 0);
            REQUIRE(undoStack.index() == acore::INVALID_INDEX);
            REQUIRE(undoStack.isClean());
        }
    }
}

TEST_CASE("count() const noexcept -> size_type [acore::UndoStack]")
{
    SECTION("[empty]")
    {
        const acore::UndoStack<Command, OtherCommand> undoStack;
        REQUIRE(noexcept(undoStack.count()));
        REQUIRE(undoStack.count() == 0);
    }

    SECTION("[data]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        undoStack.push(OtherCommand{&counter});

        SECTION("[clean]")
        {
            REQUIRE(std::as_const(undoStack).count() == 2);
        }

        SECTION("[undone]")
        {
            undoStack.undo();
            REQUIRE(std::as_const(undoStack).count() == 2);
        }

        SECTION("[pushed]")
        {
            undoStack.undo();
            undoStack.push(OtherCommand{&counter});
            REQUIRE(std::as_const(undoStack).count() == 2);
        }
    }
}

TEST_CASE("index() const noexcept -> size_type [acore::UndoStack]")
{
    SECTION("[empty]")
    {
        const acore::UndoStack<Command, OtherCommand> undoStack;
        REQUIRE(noexcept(undoStack.index()));
        REQUIRE(undoStack.index() == acore::INVALID_INDEX);
    }

    SECTION("[data]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        undoStack.push(OtherCommand{&counter});

        SECTION("[clean]")
        {
            REQUIRE(std::as_const(undoStack).index() == 1);
        }

        SECTION("[undone]")
        {
            undoStack.undo();
            REQUIRE(std::as_const(undoStack).index() == 0);
        }

        SECTION("[redone]")
        {
            undoStack.undo();
            undoStack.redo();
            REQUIRE(std::as_const(undoStack).index() == 1);
        }
    }
}

TEST_CASE("isClean() const noexcept -> bool [acore::UndoStack]")
{
    SECTION("[empty]")
    {
        const acore::UndoStack<Command, OtherCommand> undoStack;
        REQUIRE(noexcept(undoStack.isClean()));
        REQUIRE(undoStack.isClean());
    }

    SECTION("[data]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        undoStack.push(OtherCommand{&counter});
        undoStack.push(Command{&counter});

        SECTION("[clean]")
        {
            REQUIRE(std::as_const(undoStack).isClean());
        }

        SECTION("[undone]")
        {
            undoStack.undo();
            REQUIRE_FALSE(std::as_const(undoStack).isClean());
        }

        SECTION("[redone]")
        {
            undoStack.undo();
            undoStack.redo();
            REQUIRE(std::as_const(undoStack).isClean());
        }
    }
}

TEST_CASE("push(T &&command) -> void [acore::UndoStack]")
{
    SECTION("[empty]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        REQUIRE(counter.redo == 1);
        REQUIRE(counter.undo == 0);
    }

    SECTION("[data]")
    {
        acore::UndoStack<Command, OtherCommand> undoStack;
        CommandCounter counter;
        undoStack.push(Command{&counter});
        undoStack.push(OtherCommand{&counter});
        undoStack.push(Command{&counter});

        SECTION("[clean]")
        {
            undoStack.push(OtherCommand{&counter});
            REQUIRE(counter.redo == 4);
            REQUIRE(counter.undo == 0);
        }

        SECTION("[undone]")
        {
            undoStack.undo();
            undoStack.push(OtherCommand{&counter});
            REQUIRE(counter.redo == 4);
            REQUIRE(counter.undo == 1);
        }
    }
}

TEST_CASE("redo() -> void [acore::UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    undoStack.push(Command{&counter});

    SECTION("[clean]")
    {
        undoStack.redo();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 0);
    }

    SECTION("[undone]")
    {
        undoStack.undo();
        undoStack.redo();
        REQUIRE(counter.redo == 4);
        REQUIRE(counter.undo == 1);
    }
}

TEST_CASE("redoAll() -> void [acore::UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    undoStack.push(Command{&counter});

    SECTION("[clean]")
    {
        undoStack.redoAll();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 0);
    }

    SECTION("[undone]")
    {
        undoStack.undo();
        undoStack.redoAll();
        REQUIRE(counter.redo == 4);
        REQUIRE(counter.undo == 1);
    }

    SECTION("[undone all]")
    {
        undoStack.undoAll();
        undoStack.redoAll();
        REQUIRE(counter.redo == 6);
        REQUIRE(counter.undo == 3);
    }
}

TEST_CASE("undo() -> void [acore::UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    undoStack.push(Command{&counter});

    SECTION("[clean]")
    {
        undoStack.undo();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 1);
    }

    SECTION("[undone]")
    {
        undoStack.undo();
        undoStack.undo();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 2);
    }
}

TEST_CASE("undoAll() -> void [acore::UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    undoStack.push(Command{&counter});

    SECTION("[clean]")
    {
        undoStack.undoAll();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 3);
    }

    SECTION("[undone]")
    {
        undoStack.undo();
        undoStack.undoAll();
        REQUIRE(counter.redo == 3);
        REQUIRE(counter.undo == 3);
    }
}

TEST_CASE("operator=(UndoStack &&other) noexcept -> UndoStack & [acore::UndoStack]")
{
    acore::UndoStack<Command, OtherCommand> undoStack;
    CommandCounter counter;
    undoStack.push(Command{&counter});
    undoStack.push(OtherCommand{&counter});
    acore::UndoStack<Command, OtherCommand> other;
    REQUIRE(noexcept(other = std::move(undoStack))); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    other = std::move(undoStack); //NOLINT(bugprone-use-after-move, hicpp-invalid-access-moved)
    other.undo();
    REQUIRE(counter.redo == 2);
    REQUIRE(counter.undo == 1);
}

TEST_CASE("acore::UndoStack [examples]")
{
    SECTION("[usage]")
    {
        // clang-format off
//! [[Usage]]
struct C1 { void redo() { } void undo() { } };
struct C2 { void redo() { } void undo() { } };

acore::UndoStack<C1, C2> stack;
stack.push(C1{});
stack.push(C2{});

stack.undo(); //The index now points to the first command pushed onto the stack
stack.push(C1{}); //The second command pushed onto the stack is discarded and new one is pushed onto the stack as its new top
//! [[Usage]]
        // clang-format on

        REQUIRE(stack.count() == 2);
        REQUIRE(stack.index() == 1);
    }
}
}
