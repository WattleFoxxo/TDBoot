import os
import shutil

Import("env")

def copy_bootloader(source, target, env):
    custom_bootloader = os.path.join("blob", "tdboot_bootloader.bin")
    target_bootloader = os.path.join(env.subst("$BUILD_DIR"), "bootloader.bin")

    shutil.copyfile(custom_bootloader, target_bootloader)
    print("Bootloader replaced successfully.")

env.AddPreAction("upload", copy_bootloader)