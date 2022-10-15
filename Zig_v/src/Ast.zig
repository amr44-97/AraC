const Lexer = @import("Lexer.zig");

const AstNode = @This();

pub const AstType = enum { Ast_ADD, Ast_SUBSTRACT, Ast_DIVIDE, AST_MULTIPLY, Ast_INTLIT };

tok: Lexer.Token,
Intval: ?u64,
op: AstType,
left: ?*AstNode,
right: ?*AstNode,
valid: bool,
