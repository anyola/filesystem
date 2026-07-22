#include <gtest/gtest.h>

#include "directory.hpp"
#include "file.hpp"

using filesystem::directory;
using filesystem::file;
using filesystem::fsobject;

TEST(FileTest, ConstructsWithNameAndContent) {
    file f("readme.txt", "hello");
    EXPECT_EQ(f.get_name(), "readme.txt");
    EXPECT_EQ(f.get_content(), "hello");
    EXPECT_EQ(f.get_size(), 5U);
}

TEST(FileTest, DefaultContentIsEmpty) {
    file f("empty.txt");
    EXPECT_EQ(f.get_content(), "");
    EXPECT_EQ(f.get_size(), 0U);
}

TEST(FileTest, IsDirectoryReturnsFalse) {
    file f("data.bin");
    EXPECT_FALSE(f.is_directory());
}

TEST(FileTest, SetContentUpdatesSize) {
    file f("data.txt", "abc");
    f.set_content("abcdef");
    EXPECT_EQ(f.get_content(), "abcdef");
    EXPECT_EQ(f.get_size(), 6U);
}

TEST(FileTest, CloneProducesIndependentCopy) {
    file original("data.txt", "original content");
    auto cloned = original.clone();

    ASSERT_NE(cloned, nullptr);
    ASSERT_FALSE(cloned->is_directory());

    auto* cloned_file = static_cast<file*>(cloned.get());
    EXPECT_EQ(cloned_file->get_content(), "original content");

    cloned_file->set_content("modified");
    EXPECT_EQ(original.get_content(), "original content");
    EXPECT_EQ(cloned_file->get_content(), "modified");
}

TEST(FileTest, ClonePreservesName) {
    file original("report.txt", "data");
    auto cloned = original.clone();
    EXPECT_EQ(cloned->get_name(), "report.txt");
}

TEST(FileTest, GetPathAtRootIsSlashPlusName) {
    file f("solo.txt");
    EXPECT_EQ(f.get_path(), "/solo.txt");
}

TEST(IsValidNameTest, RejectsEmptyString) {
    EXPECT_FALSE(fsobject::is_valid_name(""));
}

TEST(IsValidNameTest, RejectsSingleDot) {
    EXPECT_FALSE(fsobject::is_valid_name("."));
}

TEST(IsValidNameTest, RejectsDoubleDot) {
    EXPECT_FALSE(fsobject::is_valid_name(".."));
}

TEST(IsValidNameTest, RejectsNameContainingSlash) {
    EXPECT_FALSE(fsobject::is_valid_name("a/b"));
}

TEST(IsValidNameTest, RejectsNameStartingWithSlash) {
    EXPECT_FALSE(fsobject::is_valid_name("/etc"));
}

TEST(IsValidNameTest, AcceptsOrdinaryName) {
    EXPECT_TRUE(fsobject::is_valid_name("report.txt"));
}

TEST(IsValidNameTest, AcceptsNameWithDotsInsideButNotOnly) {
    EXPECT_TRUE(fsobject::is_valid_name("..."));
    EXPECT_TRUE(fsobject::is_valid_name("a..b"));
}

TEST(IsValidNameTest, AcceptsNameWithSpacesAndUnicode) {
    EXPECT_TRUE(fsobject::is_valid_name("my report.txt"));
    EXPECT_TRUE(fsobject::is_valid_name("отчёт.txt"));
}

TEST(DirectoryValidateNameTest, TrueWhenDirectoryIsEmpty) {
    directory dir("root");
    EXPECT_TRUE(dir.validate_name("anything.txt"));
}

TEST(DirectoryValidateNameTest, FalseWhenNameTakenByDirectChild) {
    directory dir("root");
    dir.add_child(std::make_unique<file>("a.txt"));
    EXPECT_FALSE(dir.validate_name("a.txt"));
}

TEST(DirectoryValidateNameTest, TrueWhenNameTakenOnlyInNestedSubdirectory) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("a.txt"));

    EXPECT_TRUE(root.validate_name("a.txt"));
}

TEST(DirectoryValidateNameTest, FalseWhenNameMatchesAnotherDirectory) {
    directory root("root");
    root.add_child(std::make_unique<directory>("docs"));
    EXPECT_FALSE(root.validate_name("docs"));
}

TEST(DirectoryValidateNameTest, FileAndDirectoryShareSameNamespace) {
    directory root("root");
    root.add_child(std::make_unique<file>("item"));
    EXPECT_FALSE(root.validate_name("item"));
}

TEST(RenameTest, SucceedsWithValidUniqueName) {
    file f("old.txt");
    EXPECT_TRUE(f.rename("new.txt"));
    EXPECT_EQ(f.get_name(), "new.txt");
}

TEST(RenameTest, FailsOnEmptyNameAndKeepsOldName) {
    file f("valid.txt");
    EXPECT_FALSE(f.rename(""));
    EXPECT_EQ(f.get_name(), "valid.txt");
}

TEST(RenameTest, FailsOnSlashInName) {
    file f("valid.txt");
    EXPECT_FALSE(f.rename("a/b"));
    EXPECT_EQ(f.get_name(), "valid.txt");
}

TEST(RenameTest, FailsOnDotAndDotDot) {
    file f("valid.txt");
    EXPECT_FALSE(f.rename("."));
    EXPECT_FALSE(f.rename(".."));
    EXPECT_EQ(f.get_name(), "valid.txt");
}

TEST(RenameTest, FailsWhenSiblingAlreadyHasThatName) {
    directory root("root");
    root.add_child(std::make_unique<file>("taken.txt"));
    fsobject* target = root.add_child(std::make_unique<file>("free.txt"));

    ASSERT_NE(target, nullptr);
    EXPECT_FALSE(target->rename("taken.txt"));
    EXPECT_EQ(target->get_name(), "free.txt");
}

TEST(RenameTest, SucceedsWhenNameFreeAmongSiblings) {
    directory root("root");
    fsobject* target = root.add_child(std::make_unique<file>("old.txt"));

    ASSERT_NE(target, nullptr);
    EXPECT_TRUE(target->rename("new.txt"));
    EXPECT_EQ(target->get_name(), "new.txt");
    EXPECT_TRUE(root.validate_name("old.txt"));
    EXPECT_FALSE(root.validate_name("new.txt"));
}

TEST(RenameTest, RootObjectHasNoParentSoNoSiblingConflictPossible) {
    directory root("root");
    EXPECT_TRUE(root.rename("renamed_root"));
    EXPECT_EQ(root.get_name(), "renamed_root");
}

TEST(RenameTest, RenamingDirectoryDoesNotConflictWithItsOwnChildren) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("sub"));

    EXPECT_TRUE(sub->rename("renamed_sub"));
}

TEST(DirectoryTest, ConstructsEmpty) {
    directory dir("root");
    EXPECT_TRUE(dir.is_directory());
    EXPECT_EQ(dir.get_size(), 0U);
    EXPECT_TRUE(dir.find_all_current_level().empty());
}

TEST(DirectoryTest, AddChildSucceedsAndSetsParent) {
    directory dir("root");
    fsobject* added = dir.add_child(std::make_unique<file>("a.txt", "hi"));

    ASSERT_NE(added, nullptr);
    EXPECT_EQ(added->get_name(), "a.txt");
    EXPECT_EQ(added->get_parent(), &dir);
    EXPECT_EQ(added->get_path(), "/root/a.txt");
}

TEST(DirectoryTest, AddChildRejectsDuplicateNameOnSameLevel) {
    directory dir("root");
    ASSERT_NE(dir.add_child(std::make_unique<file>("a.txt")), nullptr);

    fsobject* second = dir.add_child(std::make_unique<file>("a.txt"));
    EXPECT_EQ(second, nullptr);
    EXPECT_EQ(dir.find_all_current_level().size(), 1U);
}

TEST(DirectoryTest, AddChildAllowsSameNameInDifferentSubdirectories) {
    directory root("root");
    auto* docs = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("docs")));
    auto* backup = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("backup")));

    ASSERT_NE(docs, nullptr);
    ASSERT_NE(backup, nullptr);

    EXPECT_NE(docs->add_child(std::make_unique<file>("report.txt")), nullptr);
    EXPECT_NE(backup->add_child(std::make_unique<file>("report.txt")), nullptr);
}

TEST(DirectoryTest, AddChildRejectsNullptr) {
    directory dir("root");
    EXPECT_EQ(dir.add_child(nullptr), nullptr);
}

TEST(DirectoryTest, FindLocatesNestedObject) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("deep.txt"));

    fsobject* found = root.find("deep.txt");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->get_path(), "/root/sub/deep.txt");
}

TEST(DirectoryTest, FindReturnsNullptrWhenMissing) {
    directory root("root");
    EXPECT_EQ(root.find("missing.txt"), nullptr);
}

TEST(DirectoryTest, HasChildIsRecursive) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("deep.txt"));

    EXPECT_TRUE(root.has_child("deep.txt"));
    EXPECT_FALSE(root.has_child("nope.txt"));
}

TEST(DirectoryTest, FindAllReturnsEveryNodeInSubtree) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("a.txt"));
    sub->add_child(std::make_unique<file>("b.txt"));

    auto all = root.find_all();
    EXPECT_EQ(all.size(), 3U);
}

TEST(DirectoryTest, FindAllCurrentLevelIgnoresNestedObjects) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("a.txt"));
    root.add_child(std::make_unique<file>("top.txt"));

    EXPECT_EQ(root.find_all_current_level().size(), 2U);
}

TEST(DirectoryTest, RemoveChildExtractsDirectChildAndClearsParent) {
    directory root("root");
    root.add_child(std::make_unique<file>("a.txt"));

    std::unique_ptr<fsobject> removed = root.remove_child("a.txt");
    ASSERT_NE(removed, nullptr);
    EXPECT_EQ(removed->get_parent(), nullptr);
    EXPECT_TRUE(root.find_all_current_level().empty());
}

TEST(DirectoryTest, RemoveChildReturnsNullptrWhenMissing) {
    directory root("root");
    std::unique_ptr<fsobject> removed = root.remove_child("nope.txt");
    EXPECT_EQ(removed, nullptr);
}

TEST(DirectoryTest, GetSizeSumsAllDescendantsRecursively) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    root.add_child(std::make_unique<file>("a.txt", "12345"));
    sub->add_child(std::make_unique<file>("b.txt", "1234567"));

    EXPECT_EQ(root.get_size(), 12U);
}

TEST(DirectoryTest, CloneIsDeepAndIndependent) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("a.txt", "original"));

    auto cloned = root.clone();
    auto* cloned_dir = static_cast<directory*>(cloned.get());

    fsobject* cloned_file = cloned_dir->find("a.txt");
    ASSERT_NE(cloned_file, nullptr);
    static_cast<file*>(cloned_file)->set_content("changed");

    fsobject* original_file = root.find("a.txt");
    ASSERT_NE(original_file, nullptr);
    EXPECT_EQ(static_cast<file*>(original_file)->get_content(), "original");
}

TEST(DirectoryTest, IsAncestorTrueForDirectAndNestedChildren) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    auto* leaf = static_cast<file*>(sub->add_child(std::make_unique<file>("leaf.txt")));

    EXPECT_TRUE(root.is_ancestor(*sub));
    EXPECT_TRUE(root.is_ancestor(*leaf));
}

TEST(DirectoryTest, IsAncestorFalseForUnrelatedObject) {
    directory root("root");
    directory other("other");
    EXPECT_FALSE(root.is_ancestor(other));
}

TEST(MoveTest, MovesFileBetweenDirectories) {
    directory root("root");
    auto* src_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("src")));
    auto* dst_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("dst")));
    src_dir->add_child(std::make_unique<file>("a.txt", "hi"));

    fsobject* moved = src_dir->move("a.txt", *dst_dir);

    ASSERT_NE(moved, nullptr);
    EXPECT_EQ(moved->get_parent(), dst_dir);
    EXPECT_EQ(src_dir->find_all_current_level().size(), 0U);
    EXPECT_EQ(dst_dir->find_all_current_level().size(), 1U);
}

TEST(MoveTest, ReturnsNullptrWhenObjectNotFound) {
    directory root("root");
    directory dst("dst");
    EXPECT_EQ(root.move("missing.txt", dst), nullptr);
}

TEST(MoveTest, ReturnsNullptrWhenMovingIntoSelf) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    EXPECT_EQ(root.move("sub", *sub), nullptr);
}

TEST(MoveTest, ReturnsNullptrWhenMovingIntoOwnDescendant) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    auto* nested = static_cast<directory*>(
        sub->add_child(std::make_unique<directory>("nested")));

    EXPECT_EQ(root.move("sub", *nested), nullptr);
}

TEST(MoveTest, MovedDirectoryKeepsItsChildren) {
    directory root("root");
    auto* src_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("src")));
    auto* dst_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("dst")));
    auto* payload = static_cast<directory*>(
        src_dir->add_child(std::make_unique<directory>("payload")));
    payload->add_child(std::make_unique<file>("inside.txt"));

    fsobject* moved = src_dir->move("payload", *dst_dir);

    ASSERT_NE(moved, nullptr);
    auto* moved_dir = static_cast<directory*>(moved);
    EXPECT_NE(moved_dir->find("inside.txt"), nullptr);
    EXPECT_EQ(moved_dir->find("inside.txt")->get_path(), "/root/dst/payload/inside.txt");
}

TEST(MoveTest, OriginalObjectStaysPutWhenMoveFails) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));

    fsobject* result = root.move("sub", *sub);

    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(root.find_all_current_level().size(), 1U);
    EXPECT_EQ(sub->get_parent(), &root);
}

TEST(MoveTest, ReturnsNullptrWhenDestinationAlreadyHasSameName) {
    directory root("root");
    auto* src_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("src")));
    auto* dst_dir = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("dst")));
    src_dir->add_child(std::make_unique<file>("a.txt", "from src"));
    dst_dir->add_child(std::make_unique<file>("a.txt", "already here"));

    fsobject* result = src_dir->move("a.txt", *dst_dir);

    EXPECT_EQ(result, nullptr);
    EXPECT_NE(src_dir->find_all_current_level().size(), 0U);
    ASSERT_NE(src_dir->find("a.txt"), nullptr);
    EXPECT_EQ(static_cast<file*>(src_dir->find("a.txt"))->get_content(), "from src");
}

TEST(EdgeCaseTest, EmptyDirectoryGetSizeIsZero) {
    directory dir("empty");
    EXPECT_EQ(dir.get_size(), 0U);
}

TEST(EdgeCaseTest, DeeplyNestedPathIsBuiltCorrectly) {
    directory root("root");
    auto* level1 = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("level1")));
    auto* level2 = static_cast<directory*>(
        level1->add_child(std::make_unique<directory>("level2")));
    fsobject* leaf = level2->add_child(std::make_unique<file>("leaf.txt"));

    EXPECT_EQ(leaf->get_path(), "/root/level1/level2/leaf.txt");
}

TEST(EdgeCaseTest, RootPathIsJustSlashPlusOwnName) {
    directory root("root");
    EXPECT_EQ(root.get_path(), "/root");
}

TEST(EdgeCaseTest, RemovedObjectCanBeReAddedElsewhere) {
    directory root("root");
    auto* a = static_cast<directory*>(root.add_child(std::make_unique<directory>("a")));
    auto* b = static_cast<directory*>(root.add_child(std::make_unique<directory>("b")));
    a->add_child(std::make_unique<file>("x.txt"));

    std::unique_ptr<fsobject> extracted = a->remove_child("x.txt");
    ASSERT_NE(extracted, nullptr);
    EXPECT_EQ(extracted->get_parent(), nullptr);

    fsobject* readded = b->add_child(std::move(extracted));
    ASSERT_NE(readded, nullptr);
    EXPECT_EQ(readded->get_parent(), b);
}

TEST(EdgeCaseTest, RemovingSameNameTwiceFailsSecondTime) {
    directory root("root");
    root.add_child(std::make_unique<file>("a.txt"));

    EXPECT_NE(root.remove_child("a.txt"), nullptr);
    EXPECT_EQ(root.remove_child("a.txt"), nullptr);
}

TEST(EdgeCaseTest, RemoveChildOnEmptyDirectoryReturnsNullptr) {
    directory dir("empty");
    EXPECT_EQ(dir.remove_child("nothing"), nullptr);
}

TEST(EdgeCaseTest, RemovedDirectorySubtreeStaysIntactAfterExtraction) {
    directory root("root");
    auto* sub = static_cast<directory*>(root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("inside1.txt"));
    sub->add_child(std::make_unique<file>("inside2.txt"));

    std::unique_ptr<fsobject> extracted = root.remove_child("sub");
    ASSERT_NE(extracted, nullptr);

    auto* extracted_dir = static_cast<directory*>(extracted.get());
    EXPECT_EQ(extracted_dir->find_all_current_level().size(), 2U);
}

TEST(EdgeCaseTest, IsAncestorOnEmptyDirectoryIsFalse) {
    directory dir("empty");
    file unrelated("unrelated.txt");
    EXPECT_FALSE(dir.is_ancestor(unrelated));
}

TEST(EdgeCaseTest, DirectoryIsNotItsOwnAncestorByDefaultCheck) {
    directory dir("self");
    EXPECT_FALSE(dir.is_ancestor(dir));
}

TEST(EdgeCaseTest, AddChildRejectsNullptr) {
    directory dir("root");
    EXPECT_EQ(dir.add_child(nullptr), nullptr);
    EXPECT_TRUE(dir.find_all_current_level().empty());
}

TEST(EdgeCaseTest, AddingTwoDirectoriesWithSameNameFails) {
    directory root("root");
    ASSERT_NE(root.add_child(std::make_unique<directory>("dup")), nullptr);
    EXPECT_EQ(root.add_child(std::make_unique<directory>("dup")), nullptr);
}

TEST(EdgeCaseTest, CloningEmptyDirectoryProducesEmptyIndependentCopy) {
    directory dir("empty");
    auto cloned = dir.clone();

    ASSERT_NE(cloned, nullptr);
    auto* cloned_dir = static_cast<directory*>(cloned.get());
    EXPECT_TRUE(cloned_dir->find_all_current_level().empty());
    EXPECT_EQ(cloned_dir->get_name(), "empty");
}

TEST(EdgeCaseTest, ClonedSubdirectoryHasNoSharedParentWithOriginal) {
    directory root("root");
    root.add_child(std::make_unique<directory>("sub"));

    auto cloned = root.clone();
    auto* cloned_dir = static_cast<directory*>(cloned.get());
    fsobject* cloned_sub = cloned_dir->find_all_current_level().front();

    EXPECT_EQ(cloned_sub->get_parent(), cloned_dir);
    EXPECT_NE(cloned_sub->get_parent(), &root);
}

TEST(EdgeCaseTest, ModifyingClonedTreeDoesNotAffectOriginalStructure) {
    directory root("root");
    auto* sub = static_cast<directory*>(root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("a.txt"));

    auto cloned = root.clone();
    auto* cloned_dir = static_cast<directory*>(cloned.get());

    auto* cloned_sub = static_cast<directory*>(cloned_dir->find_all_current_level().front());
    cloned_sub->remove_child("a.txt");

    EXPECT_NE(sub->find_all_current_level().size(), 0U);
    EXPECT_NE(root.find("a.txt"), nullptr);
}

TEST(EdgeCaseTest, SizeWithEmptyAndNonEmptyFilesMixed) {
    directory root("root");
    root.add_child(std::make_unique<file>("empty.txt", ""));
    root.add_child(std::make_unique<file>("full.txt", "1234567890"));

    EXPECT_EQ(root.get_size(), 10U);
}

TEST(EdgeCaseTest, SizeUnaffectedByEmptySubdirectories) {
    directory root("root");
    root.add_child(std::make_unique<directory>("empty_sub"));
    root.add_child(std::make_unique<file>("a.txt", "abc"));

    EXPECT_EQ(root.get_size(), 3U);
}

TEST(EdgeCaseTest, StressManyChildrenAtSameLevel) {
    directory root("root");
    constexpr int total = 500;
    for (int i = 0; i < total; ++i) {
        fsobject* added = root.add_child(
            std::make_unique<file>("file_" + std::to_string(i) + ".txt"));
        ASSERT_NE(added, nullptr);
    }
    EXPECT_EQ(root.find_all_current_level().size(), static_cast<std::size_t>(total));
}

TEST(EdgeCaseTest, DeepRecursionCloneAndSizeDoNotCrash) {
    directory root("root");
    directory* current = &root;
    constexpr int depth = 200;
    for (int i = 0; i < depth; ++i) {
        current = static_cast<directory*>(
            current->add_child(std::make_unique<directory>("d" + std::to_string(i))));
        current->add_child(std::make_unique<file>("f.txt", "x"));
    }

    auto cloned = root.clone();
    EXPECT_EQ(root.get_size(), static_cast<std::size_t>(depth));
    EXPECT_EQ(cloned->get_size(), static_cast<std::size_t>(depth));
}

TEST(EdgeCaseTest, StressFindAllOnWideAndDeepTree) {
    directory root("root");
    constexpr int branches = 10;
    constexpr int per_branch = 20;
    for (int b = 0; b < branches; ++b) {
        auto* branch = static_cast<directory*>(
            root.add_child(std::make_unique<directory>("branch" + std::to_string(b))));
        for (int i = 0; i < per_branch; ++i) {
            branch->add_child(std::make_unique<file>("f" + std::to_string(i) + ".txt"));
        }
    }

    auto all = root.find_all();
    EXPECT_EQ(all.size(), static_cast<std::size_t>(branches + branches * per_branch));
}

TEST(PrintTest, FilePrintsNameAndSize) {
    file f("test.txt", "hello");
    std::ostringstream oss;
    f.print(oss);
    std::string output = oss.str();
    EXPECT_NE(output.find("test.txt"), std::string::npos);
    EXPECT_NE(output.find("5 bytes"), std::string::npos);
}

TEST(PrintTest, DirectoryPrintsNameAndSize) {
    directory dir("root");
    std::ostringstream oss;
    dir.print(oss);
    std::string output = oss.str();
    EXPECT_NE(output.find("root"), std::string::npos);
    EXPECT_NE(output.find("0 bytes"), std::string::npos);
}

TEST(PrintTest, DirectoryPrintsChildren) {
    directory root("root");
    root.add_child(std::make_unique<file>("a.txt", "abc"));
    root.add_child(std::make_unique<file>("b.txt", "12345"));
    
    std::ostringstream oss;
    root.print(oss);
    std::string output = oss.str();
    
    EXPECT_NE(output.find("a.txt"), std::string::npos);
    EXPECT_NE(output.find("b.txt"), std::string::npos);
}

TEST(PrintTest, NestedDirectoryPrintsAllLevels) {
    directory root("root");
    auto* sub = static_cast<directory*>(
        root.add_child(std::make_unique<directory>("sub")));
    sub->add_child(std::make_unique<file>("deep.txt", "x"));
    
    std::ostringstream oss;
    root.print(oss);
    std::string output = oss.str();
    
    EXPECT_NE(output.find("root"), std::string::npos);
    EXPECT_NE(output.find("sub"), std::string::npos);
    EXPECT_NE(output.find("deep.txt"), std::string::npos);
}

TEST(PrintTest, PrintDoesNotCrashOnEmptyDirectory) {
    directory dir("empty");
    std::ostringstream oss;
    EXPECT_NO_THROW(dir.print(oss));
}

TEST(PrintTest, PrintDoesNotCrashOnDeepTree) {
    directory root("root");
    directory* current = &root;
    for(int i = 0; i < 100; ++i) {
        current = static_cast<directory*>(
            current->add_child(std::make_unique<directory>("d" + std::to_string(i))));
    }
    std::ostringstream oss;
    EXPECT_NO_THROW(root.print(oss));
}