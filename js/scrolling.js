const header = document.querySelector('.site-header');
let scrollCount = 0;
let lastScrollPosition = 0;
let threshold = 100;
const hysteresis = 100;
const requiredScrolls = 5;
let scrollTimeout;

window.onscroll = function () {
  if (document.documentElement.scrollHeight <= 2.5 * window.innerHeight) {
    return;
  }

  const currentScroll = document.documentElement.scrollTop || document.body.scrollTop;

  if (currentScroll > threshold) {
    if (lastScrollPosition > threshold) {
      scrollCount++;
    } else {
      scrollCount = 1;
    }
  } else {
    if (lastScrollPosition <= threshold) {
      scrollCount++;
    } else {
      scrollCount = 1;
    }
  }

  if (scrollCount >= requiredScrolls) {
    if (currentScroll > threshold) {
      header.classList.add('shrink');
      threshold = 100 - hysteresis;
    } else {
      header.classList.remove('shrink');
      threshold = 100 + hysteresis;
    }
    scrollCount = 0;
  }

  lastScrollPosition = currentScroll;

  clearTimeout(scrollTimeout);
  scrollTimeout = setTimeout(() => {
    if (currentScroll > threshold) {
      header.classList.add('shrink');
    } else {
      header.classList.remove('shrink');
    }
    threshold = 100;
    scrollCount = 0;
  }, 100);
};

const links = document.querySelectorAll('a[href^="#"]');

function adjustScroll(event) {
  event.preventDefault();
  const targetId = event.target.getAttribute('href').substring(1);
  const targetElement = document.getElementById(targetId);

  if (!targetElement) return;

  const headerHeight = header.classList.contains('shrink') ? header.offsetHeight : header.offsetHeight;

  window.scrollTo({
    top: targetElement.offsetTop - headerHeight,
    behavior: 'smooth'
  });
}

links.forEach(link => {
  link.addEventListener('click', adjustScroll);
});
