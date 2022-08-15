import React from 'react';
import UIkit from 'uikit';
import Icons from 'uikit/dist/js/uikit-icons';
import 'uikit/dist/css/uikit.min.css';
import 'uikit/dist/js/uikit.min.js';
import './App.css';

window.UIkit = UIkit;
window.UIkit.use(Icons);

const STORAGE_KEY = 'fan-controller-api-key';

function App() {
  const [apiKey, setApiKey] = React.useState('');
  const [isLoading, setIsLoading] = React.useState(false);
  const [isShowingApiKey, setIsShowingApiKey] = React.useState(false);
  const [postResponse, setPostResponse] = React.useState(null);
  const lockIconRef = React.useRef();
  const unlockIconRef = React.useRef();

  React.useEffect(() => {
    const cachedApiKey = localStorage.getItem(STORAGE_KEY);
    if (cachedApiKey) {
      setApiKey(cachedApiKey);
    }

    window.UIkit.icon(lockIconRef.current, { icon: 'lock' });
    window.UIkit.icon(unlockIconRef.current, { icon: 'unlock' });

    // eslint-disable-next-line
  }, []);

  React.useEffect(function syncApiKeyWithStorage() {
    localStorage.setItem(STORAGE_KEY, apiKey);
  }, [apiKey]);

  const handleKey = React.useCallback((event) => {
    setApiKey(event.currentTarget.value);
  }, [setApiKey]);
  const handleSubmitWithCode = React.useCallback((code) => {
    setIsLoading(true);
    fetch('https://4ntchrutj1.execute-api.us-east-2.amazonaws.com/default/fan-controller-handler', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        key: apiKey,
        message: code,
      }),
    }).then((response) => {
      response.json().then((data) => {
        if (data.statusCode === 200) {
          setPostResponse({ isError: false, message: `Success with message "${code}"` });
        } else {
          setPostResponse({ isError: true, message: `${data.statusCode}: Error with message "${code}"` });
        }
        setIsLoading(false);
      }).catch((err) => {
        setPostResponse({ isError: true, message: err.message });
        setIsLoading(false);
      })
    }).catch((err) => {
      setPostResponse({ isError: true, message: err.message })
      setIsLoading(false);
    });
  }, [apiKey, setIsLoading, setPostResponse]);
  const toggleApiKeyVisibility = React.useCallback((evt) => {
    setIsShowingApiKey((current) => !current);
  }, [setIsShowingApiKey]);

  return (<>
    <div className="uk-container uk-padding uk-align-center uk-width-1-2@m">
      <div className="uk-margin-large uk-flex">
        <label className="uk-flex-none uk-margin-right uk-margin-small-top uk-form-label" htmlFor="txtKey">Key:</label>
        <div className="uk-inline uk-flex-1">
          <button
            className="uk-form-icon uk-form-icon-flip"
            onClick={toggleApiKeyVisibility}
            ref={lockIconRef}
            {...(isShowingApiKey ? { hidden: true } : {})}
          />
          <button
            className="uk-form-icon uk-form-icon-flip"
            onClick={toggleApiKeyVisibility}
            ref={unlockIconRef}
            {...(isShowingApiKey ? {} : { hidden: true })}
          />
          <input
            className="uk-input"
            id="txtKey"
            name="key"
            onChange={handleKey}
            type={isShowingApiKey ? 'text' : 'password'}
            value={apiKey}
          />
        </div>
      </div>
      <div className="uk-margin uk-flex uk-flex-column">
        <button className="uk-button uk-button-primary uk-margin-small" disabled={isLoading} onClick={() => handleSubmitWithCode('light')}>Light</button>
        <button className="uk-button uk-button-primary uk-margin-small" disabled={isLoading} onClick={() => handleSubmitWithCode('off')}>Off</button>
        <button className="uk-button uk-button-primary uk-margin-small" disabled={isLoading} onClick={() => handleSubmitWithCode('speed1')}>Speed 1</button>
        <button className="uk-button uk-button-primary uk-margin-small" disabled={isLoading} onClick={() => handleSubmitWithCode('speed2')}>Speed 2</button>
        <button className="uk-button uk-button-primary uk-margin-small" disabled={isLoading} onClick={() => handleSubmitWithCode('speed3')}>Speed 3</button>
      </div>
    </div>
    <div className="uk-container-large uk-align-center uk-width-1-2@m">
      <div className={['uk-alert', postResponse?.isError ? 'uk-alert-danger' : 'uk-alert-success'].join(' ')}>
        {isLoading ? <div className="uk-spinner" /> : postResponse?.message}
      </div>
    </div>
    <a href="https://www.flaticon.com/free-icons/fan" title="fan icons">Fan icons created by yoyonpujiono - Flaticon</a>
  </>);
}

export default App;
