function enhanceHeaderControls() {
  const controls = [
    ["label.md-header__button[for='__drawer']", "navigation"],
    ["label.md-header__button[for='__search']", "search"],
  ];

  for (const [selector, name] of controls) {
    for (const control of document.querySelectorAll(selector)) {
      const toggle = document.getElementById(control.htmlFor);
      if (!(toggle instanceof HTMLInputElement)) continue;

      const updateState = () => {
        control.setAttribute("aria-expanded", String(toggle.checked));
        control.setAttribute(
          "aria-label",
          `${toggle.checked ? "Close" : "Open"} ${name}`,
        );
      };

      control.setAttribute("role", "button");
      control.setAttribute("tabindex", "0");
      updateState();

      if (control.dataset.keyboardEnhanced === "true") continue;
      control.dataset.keyboardEnhanced = "true";
      toggle.addEventListener("change", updateState);

      control.addEventListener("keydown", (event) => {
        if (event.key !== "Enter" && event.key !== " ") return;
        event.preventDefault();
        toggle.click();
      });
    }
  }
}

if (typeof document$ !== "undefined") {
  document$.subscribe(enhanceHeaderControls);
} else if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", enhanceHeaderControls);
} else {
  enhanceHeaderControls();
}
