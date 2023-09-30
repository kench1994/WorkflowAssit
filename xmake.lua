add_rules("mode.debug", "mode.releasedbg")


add_requires("conan::zeromq/4.3.4", {
    alias = "zeromq",
    configs = {shared = true, pic = true, vs_runtime = "MD"}
})

add_requires("conan::czmq/4.2.1", {
    alias = "czmq",
    configs = {shared = true, pic = true, vs_runtime = "MD"}
})

target("WorkflowAssit")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("zeromq", "czmq")
    add_syslinks("pthread")

    -- 自动生成 compile_commands.json 帮助代码补全跳转
    after_build(function (target)
        import("core.base.task")
        task.run("project", {kind = "compile_commands", outputdir = ".vscode"})
    end)
target_end()
