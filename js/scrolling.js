// Prevent the browser from automatically restoring scroll position on reload.
// Without this, the browser fires scroll events during page initialization
// with an unstable scrollY, which causes flickering on first load.
history.scrollRestoration = 'manual';

const header = document.querySelector('.site-header');

// ─── Navbar: shrink on scroll ─────────────────────────────────────────────────

// The header is 211px tall normally and 97px when shrunk: a difference of 114px.
// When the header changes state, sticky positioning causes the content to shift
// by that same amount, making scrollY jump 114px up or down.
// The dead zone (SHRINK_AT - UNSHRINK_AT) must be larger than 114px to prevent
// that jump from triggering the opposite state and causing a flicker loop.
const SHRINK_AT   = 200; // px: threshold to activate shrink (scrolling down)
const UNSHRINK_AT =  60; // px: threshold to deactivate shrink (scrolling up)
                         // Dead zone = 140px > 114px jump → no flicker possible

let isShrunk = false;
let ticking  = false;

function updateHeader() {
  const scrollY = window.scrollY || document.documentElement.scrollTop;

  if (!isShrunk && scrollY > SHRINK_AT) {
    header.classList.add('shrink');
    isShrunk = true;
  } else if (isShrunk && scrollY < UNSHRINK_AT) {
    header.classList.remove('shrink');
    isShrunk = false;
  }

  ticking = false;
}

// Run on load to set the correct initial state
updateHeader();

window.addEventListener('scroll', () => {
  if (!ticking) {
    requestAnimationFrame(updateHeader);
    ticking = true;
  }
}, { passive: true });

// ─── Smooth scroll for anchor links (#) ──────────────────────────────────────

const links = document.querySelectorAll('a[href^="#"]');

function adjustScroll(event) {
  event.preventDefault();

  const targetId = event.currentTarget.getAttribute('href').substring(1);
  const targetElement = document.getElementById(targetId);
  if (!targetElement) return;

  // offsetHeight reflects the actual current height (shrunk or not)
  window.scrollTo({
    top: targetElement.offsetTop - header.offsetHeight,
    behavior: 'smooth'
  });
}

links.forEach(link => link.addEventListener('click', adjustScroll));
