// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_ATOM_BROWSER_MAIN_PARTS_H_
#define ATOM_BROWSER_ATOM_BROWSER_MAIN_PARTS_H_

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/command_line.h"
#include "base/memory/memory_pressure_listener.h"
#include "base/timer/timer.h"
#include "brightray/browser/browser_main_parts.h"
#include "build/build_config.h"
#include "content/public/browser/browser_context.h"
#include "content/public/common/main_function_params.h"

class BrowserProcessImpl;
class ChromeBrowserMainExtraParts;

namespace brightray {
class BrowserContext;
}

class PrefService;

namespace atom {

class AtomBindings;
class Browser;
class JavascriptEnvironment;
class NodeBindings;
class NodeDebugger;
class BridgeTaskRunner;

class AtomBrowserMainParts : public brightray::BrowserMainParts {
 public:
  explicit AtomBrowserMainParts(const content::MainFunctionParams& parameters);
  virtual ~AtomBrowserMainParts();

  static AtomBrowserMainParts* Get();

  // Sets the exit code, will fail if the message loop is not ready.
  bool SetExitCode(int code);

  // Gets the exit code
  int GetExitCode();

  // Register a callback that should be destroyed before JavaScript environment
  // gets destroyed.
  // Returns a closure that can be used to remove |callback| from the list.
  base::Closure RegisterDestructionCallback(const base::Closure& callback);

  Browser* browser() { return browser_.get(); }

  // Add additional ChromeBrowserMainExtraParts.
  virtual void AddParts(ChromeBrowserMainExtraParts* parts);

 protected:
  // content::BrowserMainParts:
  int PreCreateThreads() override;
  int PreEarlyInitialization() override;
  void PostEarlyInitialization() override;
  void PreMainMessageLoopRun() override;
  bool MainMessageLoopRun(int* result_code) override;
  void PostMainMessageLoopStart() override;
  void PostMainMessageLoopRun() override;
  void PostDestroyThreads() override;
#if defined(OS_MACOSX) || defined(OS_WIN)
  void PreMainMessageLoopStart() override;
#endif
  void ServiceManagerConnectionStarted(
      content::ServiceManagerConnection* connection) override;

  void OnMemoryPressure(
      base::MemoryPressureListener::MemoryPressureLevel memory_pressure_level);
  void IdleHandler();

 private:
#if defined(OS_POSIX)
  // Set signal handlers.
  void HandleSIGCHLD();
  void HandleShutdownSignals();
#endif

#if defined(OS_MACOSX)
  void FreeAppDelegate();
#endif

  brightray::BrowserContext* browser_context_;

  // A fake BrowserProcess object that used to feed the source code from chrome.
  std::unique_ptr<BrowserProcessImpl> fake_browser_process_;

  // The gin::PerIsolateData requires a task runner to create, so we feed it
  // with a task runner that will post all work to main loop.
  scoped_refptr<BridgeTaskRunner> bridge_task_runner_;

  // Pointer to exit code.
  int* exit_code_;

  const content::MainFunctionParams parameters_;
  const base::CommandLine& parsed_command_line_;

  std::unique_ptr<Browser> browser_;
  std::unique_ptr<JavascriptEnvironment> js_env_;
  std::unique_ptr<NodeBindings> node_bindings_;
  std::unique_ptr<AtomBindings> atom_bindings_;

  base::RepeatingTimer gc_timer_;
  std::unique_ptr<base::MemoryPressureListener> memory_pressure_listener_;

  // Members needed across shutdown methods.
  bool restart_last_session_ = false;

  // List of callbacks should be executed before destroying JS env.
  std::list<base::Closure> destructors_;

  static AtomBrowserMainParts* self_;

  // Vector of additional ChromeBrowserMainExtraParts.
  // Parts are deleted in the inverse order they are added.
  std::vector<ChromeBrowserMainExtraParts*> chrome_extra_parts_;

  DISALLOW_COPY_AND_ASSIGN(AtomBrowserMainParts);
};

}  // namespace atom

#endif  // ATOM_BROWSER_ATOM_BROWSER_MAIN_PARTS_H_
