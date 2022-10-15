const std = @import("std");

const File = std.fs.File;
const Out = std.io.getStdOut().writer();
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();
const Parser = @import("Parser.zig");
//const Lexer = @import("Lexer.zig");
const os = std.os;
//const process = std.process;

pub fn main() !void {
    var iter = std.process.args();
    //var iter = arg.init();
    _ = iter.skip();
    var arg = iter.next();
    if (arg == null) {
        try Out.print("[Error]: Not Enough Arguments\n", .{});
        return;
    }
    var fp: File = try std.fs.cwd().openFile(arg.?, .{ .mode = .read_write });
    const fp_s = try fp.getEndPos();
    var txtbuf = try fp.readToEndAlloc(allocator, fp_s);

    var Parse = try Parser.init(allocator, txtbuf);
    defer Parse.deinit();

    try Parse.lexer.printBadTokens();
    try Out.print("-------------------------------------------------\n", .{});
    try Out.print("-------------------------------------------------\n", .{});
    try Out.print("-------------------------------------------------\n", .{});
    try Parse.lexer.printTokens();
}

test "Alloc" {}
