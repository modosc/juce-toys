#pragma once
/** Display a tree. */
class ValueTreeDebuggerMain : public Component {
public:
  class PropertyEditor : public PropertyPanel {
  public:
    PropertyEditor() { noEditValue = "not editable"; }

    const Identifier id{"id"};
    const Identifier description{"description"};

    void setSource(ValueTree &newSource) {
      clear();

      tree = newSource;

      const int maxChars = 200;

      Array<PropertyComponent *> pc;

      for (int i = 0; i < tree.getNumProperties(); ++i) {
        const Identifier name = tree.getPropertyName(i).toString();
        Value v = tree.getPropertyAsValue(name, nullptr);
        TextPropertyComponent *tpc;

        bool isEditable{true};
        if (name == id)
          isEditable = false;

        bool isMultiline{false};
        if (name == description)
          isMultiline = true;

        if (v.getValue().isObject()) {
          tpc = new TextPropertyComponent(
              noEditValue, name.toString(), maxChars, isMultiline, isEditable);
          tpc->setEnabled(false);
        } else {
          tpc = new TextPropertyComponent(
              v, name.toString(), maxChars, isMultiline, isEditable);
        }

        pc.add(tpc);
      }

      addProperties(pc);
    }

  private:
    Value noEditValue;
    ValueTree tree;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyEditor)
  };

  class Item : public TreeViewItem, public ValueTree::Listener {
  public:
    Item(PropertyEditor *_propertiesEditor, ValueTree _tree)
        : propertiesEditor(_propertiesEditor), t(_tree) {
      t.addListener(this);
    }

    ~Item() { clearSubItems(); }

    bool mightContainSubItems() { return t.getNumChildren() > 0; }

    void itemOpennessChanged(bool isNowOpen) {
      if (isNowOpen)
        updateSubItems();
    }

    void updateSubItems() {
      std::unique_ptr<XmlElement> openness = getOpennessState();
      clearSubItems();
      int children = t.getNumChildren();

      for (int i = 0; i < children; ++i)
        addSubItem(new Item(propertiesEditor, t.getChild(i)));

      if (openness.get())
        restoreOpennessState(*openness.get());
    }

    void paintItem(Graphics &g, int w, int h) {
      Font font;
      Font smallFont(11.0);
      if (isSelected())
        g.fillAll(base0);

      const float padding = 20.0f;

      String typeName = t.getType().toString();

      const float nameWidth = font.getStringWidthFloat(typeName);
      const float propertyX = padding + nameWidth;

      g.setColour(isSelected() ? base04 : base0);

      g.setFont(font);

      g.drawText(
          t.getType().toString(), 0, 0, w, h, Justification::left, false);
      // g.setColour(Colours::blue);

      String propertySummary;

      for (int i = 0; i < t.getNumProperties(); ++i) {
        const Identifier name = t.getPropertyName(i).toString();
        String propertyValue = t.getProperty(name).toString();
        propertySummary += " " + name.toString() + "=" + propertyValue;
      }

      g.drawText(propertySummary,
                 (int)rint(propertyX),
                 0,
                 w - (int)rint(propertyX),
                 h,
                 Justification::left,
                 true);
    }

    void itemSelectionChanged(bool isNowSelected) {
      if (isNowSelected) {
        t.removeListener(this);
        propertiesEditor->setSource(t);
        t.addListener(this);
      }
    }

    /* Enormous list of ValueTree::Listener options... */
    void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged,
                                  [[maybe_unused]] const Identifier &property) {
      if (t != treeWhosePropertyHasChanged)
        return;

      t.removeListener(this);
      //            if (isSelected())
      //                propertiesEditor->setSource(t);
      repaintItem();
      t.addListener(this);
    }

    void
    valueTreeChildAdded(ValueTree &parentTree,
                        [[maybe_unused]] ValueTree &childWhichHasBeenAdded) {
      if (parentTree == t)
        updateSubItems();

      treeHasChanged();
    }

    void
    valueTreeChildRemoved(ValueTree &parentTree,
                          [[maybe_unused]] ValueTree &childWhichHasBeenRemoved,
                          int) {
      if (parentTree == t)
        updateSubItems();

      treeHasChanged();
    }
    void valueTreeChildOrderChanged(ValueTree &parentTreeWhoseChildrenHaveMoved,
                                    int, int) {
      if (parentTreeWhoseChildrenHaveMoved == t)
        updateSubItems();

      treeHasChanged();
    }
    void valueTreeParentChanged(
        [[maybe_unused]] ValueTree &treeWhoseParentHasChanged) {
      treeHasChanged();
    }
    void valueTreeRedirected(ValueTree &treeWhichHasBeenChanged) {
      if (treeWhichHasBeenChanged == t)
        updateSubItems();

      treeHasChanged();
    }

    /* Works only if the ValueTree isn't updated between calls to getUniqueName.
     */
    String getUniqueName() const {
      if (!t.getParent().isValid())
        return "1";

      return String(t.getParent().indexOf(t));
    }
    const Colour base05{0, 23, 33};
    const Colour base04{0, 33, 42};
    const Colour base03{0, 43, 54};
    const Colour base02{7, 54, 66};
    const Colour base01{88, 110, 117};
    const Colour base00{101, 123, 131};
    const Colour base0{131, 148, 150};
    const Colour base1{147, 161, 161};
    const Colour base2{238, 232, 213};
    const Colour base3{253, 246, 227};

  private:
    PropertyEditor *propertiesEditor;
    ValueTree t;
    Array<Identifier> currentProperties;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Item)
  };

  ValueTreeDebuggerMain() : layoutResizer(&layout, 1, false) {
    layout.setItemLayout(0, -0.1, -0.9, -0.6);
    layout.setItemLayout(1, 5, 5, 5);
    layout.setItemLayout(2, -0.1, -0.9, -0.4);

    setSize(1000, 700);
    addAndMakeVisible(treeView);
    addAndMakeVisible(propertyEditor);
    addAndMakeVisible(layoutResizer);
  }
  ~ValueTreeDebuggerMain() override { treeView.setRootItem(nullptr); }

  void resized() override {
    Component *comps[] = {&treeView, &layoutResizer, &propertyEditor};
    layout.layOutComponents(
        comps, 3, 0, 0, getWidth(), getHeight(), true, true);
  }

  void setTree(ValueTree newTree) {
    if (!newTree.isValid()) {
      treeView.setRootItem(nullptr);
    } else if (tree != newTree) {
      tree = newTree;
      rootItem = std::make_unique<Item>(&propertyEditor, tree);
      treeView.setRootItem(rootItem.get());
    }
  }

public:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueTreeDebuggerMain)

  std::unique_ptr<Item> rootItem;
  ValueTree tree;
  TreeView treeView;
  PropertyEditor propertyEditor;
  StretchableLayoutManager layout;
  StretchableLayoutResizerBar layoutResizer;
};
