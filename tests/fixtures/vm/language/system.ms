if (system.getArchBits() == 64 || system.getArchBits() == 32) {
    io.print("Valid architecture");
}

if (#system.getMelonVersion() > 0) {
    io.print("Valid melon version");
}