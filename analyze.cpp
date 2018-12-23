#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "clang-c/Index.h"

#define assert(expr) if(!(expr)) { *((int *)0) = 0; }

enum Depth_State
{
	UNVISITED = 0,
	VISITED_FIRST_OP_AND_IS_NOT_TYPE,
	VISITED_FIRST_OP_AND_IS_TYPE,
};

static CXTypeKind target_type = CXTypeKind::CXType_NullPtr;

#define MAX_AST_DEPTH (1024*1024)
static uint8_t depth_states[MAX_AST_DEPTH];
static char *target_type_name = 0;
static bool did_compare_target_types = false;

static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, void *depth_ptr)
{
	uint64_t ast_depth = (uint64_t)depth_ptr;
	assert(ast_depth < MAX_AST_DEPTH);

	CXCursorKind cursor_kind = clang_getCursorKind(cursor);
	CXCursorKind parent_kind = clang_getCursorKind(parent);
	CXType parent_type = clang_getCursorType(parent);

	if (parent_kind == CXCursorKind::CXCursor_BinaryOperator && parent_type.kind == CXTypeKind::CXType_Bool)
	{
		CXType type = clang_getCursorType(cursor);
		bool is_pointer = (type.kind == CXTypeKind::CXType_Pointer) || (type.kind == CXTypeKind::CXType_MemberPointer) || (type.kind == CXType_BlockPointer);
		if (target_type == CXTypeKind::CXType_NullPtr && is_pointer)
		{
			CXType pointee_type = clang_getPointeeType(type);
			const char *spelling = (const char *)clang_getTypeSpelling(pointee_type).data;
			if (!strcmp(spelling, target_type_name))
			{
				target_type = pointee_type.kind;
			}
		}

		int is_target = is_pointer && (clang_getPointeeType(type).kind == target_type);
		if(depth_states[ast_depth] == UNVISITED)
		{
			if (is_target)
			{
				depth_states[ast_depth] = VISITED_FIRST_OP_AND_IS_TYPE;
			}
			else
			{
				depth_states[ast_depth] = VISITED_FIRST_OP_AND_IS_NOT_TYPE;
			}
		}
		else
		{
			if (depth_states[ast_depth] == VISITED_FIRST_OP_AND_IS_TYPE && is_target)
			{
				CXSourceLocation location = clang_getCursorLocation(parent);
				CXFile file;
				unsigned int line, col, offset;
				clang_getSpellingLocation(location, &file, &line, &col, &offset);
				CXString file_name = clang_getFileName(file);
				printf("Comparing %s pointers in %s on line %u\n", target_type_name, file_name.data, line);
				did_compare_target_types = true;
			}

			depth_states[ast_depth] = UNVISITED;
		}
	}
	
	clang_visitChildren(cursor, visitor, (void *)(ast_depth + 1));

	return CXChildVisit_Continue;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: analyze main.cpp TypeName");
		exit(1);
	}

	memset(depth_states, 0, sizeof(depth_states));
	target_type_name = argv[2];

	CXIndex index = clang_createIndex(0, 0);
  	CXTranslationUnit unit = clang_parseTranslationUnit(index, argv[1], 0, 0, 0, 0, CXTranslationUnit_None);
	if (unit)
	{
		CXCursor cursor = clang_getTranslationUnitCursor(unit);
		clang_visitChildren(cursor, visitor, 0);

		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);

		if (!did_compare_target_types)
		{
			printf("No pointer comparisons of type %s detected\n", target_type_name);
		}
		else
		{
			exit(1);
		}
	}
	else
	{
		printf("Failed to parse %s\n", argv[1]);
		exit(1);
	}
}