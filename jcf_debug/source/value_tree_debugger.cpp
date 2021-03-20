//
// Created by Jim Credland on 21/06/2015.
//

ValueTreeDebugger::ValueTreeDebugger()
    : DocumentWindow("Value Tree Debugger", Colours::lightgrey,
                     DocumentWindow::allButtons) {
  construct();
}

ValueTreeDebugger::ValueTreeDebugger(ValueTree &tree)
    : DocumentWindow("Value Tree Debugger", Colours::lightgrey,
                     DocumentWindow::allButtons) {
  construct();
  setSource(tree);
}

void ValueTreeDebugger::construct() {
  main = std::make_unique<ValueTreeDebuggerMain>();
  setContentNonOwned(main.get(), true);
  setResizable(true, false);
  setUsingNativeTitleBar(true);
  centreWithSize(getWidth(), getHeight());
  setVisible(true);
}

ValueTreeDebugger::~ValueTreeDebugger() { main->setTree(ValueTree()); }

void ValueTreeDebugger::closeButtonPressed() { setVisible(false); }

void ValueTreeDebugger::setSource(ValueTree &v) { main->setTree(v); }
